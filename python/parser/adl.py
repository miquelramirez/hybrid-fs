"""
    Convert ADL elements to FD equivalents
"""

from . import pddl
from .asp import problem as sproblem
from .exceptions import UnimplementedFeature


def ensure_conjunction(node):
    # In case we have a single atom, we wrap it on a conjunction
    if isinstance(node, sproblem.PredicateCondition):
        node = sproblem.AndCondition([node])
    return node


def _process_adl_predicate_condition(condition):
    symbol = condition.pred.name
    args = condition.variables
    sign = condition.sign
    if sign:
        return pddl.Atom(symbol, args)
    return pddl.NegatedAtom(symbol, args)


def _process_adl_conjunction(formula):
    parts = []
    for p in ensure_conjunction(formula).conditions:
        parts.append(_process_adl_predicate_condition(p))
    return pddl.Conjunction(parts)


def process_adl_flat_formula(formula):
    parts = []
    for prec in formula:
        symbol = prec[0].pred.name
        grounding = prec[0].ground_conditions[prec[1]]
        sign = prec[2]
        if sign:
            parts.append(pddl.Atom(symbol, grounding))
        else:
            parts.append(pddl.NegatedAtom(symbol, grounding))
    return pddl.Conjunction(parts)


def convert_adl_effect(effect):
    """
        Return a FD effect (or set of effects) corresponding to the given ADL effect
    """
    effs = []
    if isinstance(effect, sproblem.PredicateCondition):
        # params = [TypedObject(v, action.param_types[v].name) for v in effect.variables]
        effs.append(pddl.Effect([], pddl.Truth(), _process_adl_predicate_condition(effect)))
    elif isinstance(effect, sproblem.ConditionalEffect):
        eff_prec = _process_adl_conjunction(effect.condition)
        for ceff_formula in effect.effects:
            # params = [TypedObject(v, action.param_types[v].name) for v in ceff_formula.variables]
            effs.append(pddl.Effect([], eff_prec, _process_adl_predicate_condition(ceff_formula)))
    elif isinstance(effect, sproblem.ForAllCondition):
        parameter = pddl.TypedObject(effect.var, effect.v_type.name)
        nested = convert_adl_effect(effect.condition)
        for neff in nested:
            if isinstance(neff.condition, pddl.Conjunction):  # Otherwise we need to rethink this a bit
                for part in neff.condition.parts :
                    effs.append(pddl.Effect([parameter], part, neff.literal))
            elif isinstance(neff.condition, pddl.Atom):
                effs.append(pddl.Effect([parameter], pddl.Truth(), neff.condition))

    else:
        raise UnimplementedFeature('TODO - Effect type not yet supported')
    return effs


def convert_adl_action(action):
    """
        Convert an ADL action to FD format
    """
    action_params = [pddl.TypedObject(p, t.name) for p, t in action.parameters]
    precs = _process_adl_conjunction(action.precondition)
    cost = 1
    effs = []

    for effect in action.effects:
        if isinstance(effect, sproblem.IncreaseCondition):  # We treat the action-cost effect differently
            # ATM this will get soon pruned, as we are ignoring action costs, so this is perhaps not 100% correct.
            head = pddl.f_expression.PrimitiveNumericExpression(effect.var.name, effect.var.arguments)
            exp = effect.value
            cost = pddl.f_expression.Increase(head, exp)
        else:
            effs += convert_adl_effect(effect)

    return pddl.Action(action.name, action_params, 0, precs, effs, cost)


def convert_functions_to_fd(symbols):
    converted = []
    for symbol in symbols:
        arguments = [pddl.TypedObject(arg.name, arg.type.name) for arg in symbol.arguments]
        converted.append(pddl.functions.TypedFunction(symbol.name, arguments, symbol.function_type))
    return converted


def convert_predicates_to_fd(symbols):
    converted = []
    for symbol in symbols:
        arguments = [pddl.TypedObject(arg.name, arg.type.name) for arg in symbol.arguments]
        converted.append(pddl.Predicate(symbol.name, arguments))
    return converted
