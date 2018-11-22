from __future__ import print_function

from . import conditions
from . import pddl_types
from . import f_expression
import copy

def cartesian_product(*sequences):
    # TODO: Also exists in tools.py outside the pddl package (defined slightly
    #       differently). Not good. Need proper import paths.
    if not sequences:
        yield ()
    else:
        for tup in cartesian_product(*sequences[1:]):
            for item in sequences[0]:
                yield (item,) + tup

def parse_effects(alist, result):
    """Parse a PDDL effect (any combination of simple, conjunctive, conditional, and universal)."""
    tmp_effect = parse_effect(alist)
    normalized = tmp_effect.normalize()
    cost_eff, rest_effect = normalized.extract_cost()
    add_effect(rest_effect, result)
    if cost_eff:
        return cost_eff.effect
    else:
        return None

def add_effect(tmp_effect, result):
    """tmp_effect has the following structure:
       [ConjunctiveEffect] [UniversalEffect] [ConditionalEffect] SimpleEffect."""

    if isinstance(tmp_effect, ConjunctiveEffect):
        for effect in tmp_effect.effects:
            add_effect(effect, result)
        return
    else:
        parameters = []
        condition = conditions.Truth()
        if isinstance(tmp_effect, UniversalEffect):
            parameters = tmp_effect.parameters
            if isinstance(tmp_effect.effect, ConditionalEffect):
                condition = tmp_effect.effect.condition
                if isinstance(tmp_effect.effect.effect, SimpleEffect) :
                    effect = tmp_effect.effect.effect.effect
                elif isinstance(tmp_effect.effect.effect, AssignmentEffect) :
                    effect = tmp_effect.effect.effect
                else :
                    raise RuntimeError("Syntax Error: effect expr of conditional effects can only be a SimpleEffect or an Assignment Effect")
            elif isinstance(tmp_effect.effect, SimpleEffect) :
                effect = tmp_effect.effect.effect
            else :
                assert isinstance(tmp_effect.effect, AssignmentEffect)
                effect = tmp_effect.effect
        elif isinstance(tmp_effect, ConditionalEffect):
            condition = tmp_effect.condition
            eff = tmp_effect.effect
            assert isinstance(eff, (SimpleEffect, AssignmentEffect))
            effect = eff.effect if isinstance(eff, SimpleEffect) else eff
        elif isinstance(tmp_effect, AssignmentEffect):
            effect = tmp_effect
        else:
            assert isinstance(tmp_effect, SimpleEffect)
            effect = tmp_effect.effect

        if isinstance(effect, conditions.Literal):
            # Check for contradictory effects
            condition = condition.simplified()
            new_effect = Effect(parameters, condition, effect)
            contradiction = Effect(parameters, condition, effect.negate())
            if not contradiction in result:
                result.append(new_effect)
            else:
                # We use add-after-delete semantics, keep positive effect
                if isinstance(contradiction.literal, conditions.NegatedAtom):
                    result.remove(contradiction)
                    result.append(new_effect)
        else:
            assert isinstance(effect, AssignmentEffect)
            new_effect = Effect(parameters, condition, effect)
            # TODO - Check that no other functional assignment assigns a value to the same function point (at least sintactically)
            # TODO - Raise a warning if that's the case
            result.append(new_effect)

def parse_effect(alist):
    tag = alist[0]
    if tag == "and":
        return ConjunctiveEffect([parse_effect(eff) for eff in alist[1:]])
    elif tag == "forall":
        assert len(alist) == 3
        parameters = pddl_types.parse_typed_list(alist[1])
        effect = parse_effect(alist[2])
        return UniversalEffect(parameters, effect)
    elif tag == "when":
        assert len(alist) == 3
        condition = conditions.parse_condition(alist[1])
        effect = parse_effect(alist[2])
        return ConditionalEffect(condition, effect)
    elif tag == "increase":
        assert len(alist) == 3
        #assert alist[1] == ['total-cost']
        assignment = f_expression.parse_assignment(alist)
        return CostEffect(assignment)
    elif tag in ["assign"]:
        if len(alist) != 3 :
            message = ["Syntax Error: invalid assign effect: <eff_expr> := (assign <lhs expr> <rhs expr>)"]
            message += ["Tokens:"]
            message += [str(alist)]
            raise RuntimeError('\n'.join(message))
        lhs, rhs = f_expression.parse_functional_assignment(alist)
        return AssignmentEffect(lhs, rhs)
    else:
        return SimpleEffect(conditions.parse_literal(alist))


class Effect(object):
    def __init__(self, parameters, condition, literal):
        self.parameters = parameters
        self.condition = condition
        self.literal = literal
    def __eq__(self, other):
        return (self.__class__ is other.__class__ and
                self.parameters == other.parameters and
                self.condition == other.condition and
                self.literal == other.literal)
    def dump(self):
        indent = "  "
        if self.parameters:
            print("%sforall %s" % (indent, ", ".join(map(str, self.parameters))))
            indent += "  "
        if self.condition != conditions.Truth():
            print("%sif" % indent)
            self.condition.dump(indent + "  ")
            print("%sthen" % indent)
            indent += "  "
        print("%s%s" % (indent, self.literal))
    def copy(self):
        return Effect(self.parameters, self.condition, self.literal)
    def uniquify_variables(self, type_map):
        renamings = {}
        self.parameters = [par.uniquify_name(type_map, renamings)
                           for par in self.parameters]
        #print(self.condition)
        self.condition = self.condition.uniquify_variables(type_map, renamings)
        #print(type(self.literal))
        self.literal = self.literal.rename_variables(renamings)
    def instantiate(self, var_mapping, init_facts, fluent_facts,
                    objects_by_type, result):
        if self.parameters:
            var_mapping = var_mapping.copy() # Will modify this.
            object_lists = [objects_by_type.get(par.type, [])
                            for par in self.parameters]
            for object_tuple in cartesian_product(*object_lists):
                for (par, obj) in zip(self.parameters, object_tuple):
                    var_mapping[par.name] = obj
                self._instantiate(var_mapping, init_facts, fluent_facts, result)
        else:
            self._instantiate(var_mapping, init_facts, fluent_facts, result)
    def _instantiate(self, var_mapping, init_facts, fluent_facts, result):
        condition = []
        try:
            self.condition.instantiate(var_mapping, init_facts, fluent_facts, condition)
        except conditions.Impossible:
            return
        effects = []
        self.literal.instantiate(var_mapping, init_facts, fluent_facts, effects)
        assert len(effects) <= 1
        if effects:
            result.append((condition, effects[0]))
    def relaxed(self):
        if self.literal.negated:
            return None
        else:
            return Effect(self.parameters, self.condition.relaxed(), self.literal)
    def simplified(self):
        return Effect(self.parameters, self.condition.simplified(), self.literal)


class ConditionalEffect(object):
    def __init__(self, condition, effect):
        if isinstance(effect, ConditionalEffect):
            self.condition = conditions.Conjunction([condition, effect.condition])
            self.effect = effect.effect
        else:
            self.condition = condition
            self.effect = effect
    def dump(self, indent="  "):
        print("%sif" % (indent))
        self.condition.dump(indent + "  ")
        print("%sthen" % (indent))
        self.effect.dump(indent + "  ")
    def normalize(self):
        norm_effect = self.effect.normalize()
        if isinstance(norm_effect, ConjunctiveEffect):
            new_effects = []
            for effect in norm_effect.effects:
                supported_effect_types = (SimpleEffect, AssignmentEffect, ConditionalEffect)
                if type(effect) not in supported_effect_types :
                    message = []
                    message += ["Syntax Error: normalised conditional effect expression is not supported."]
                    message += ["Supported Types:"]
                    message += [str(t) for t in supported_effect_types]
                    raise RuntimeError( '\n'.join(message))
                new_effects.append(ConditionalEffect(copy.deepcopy(self.condition), effect))
            return ConjunctiveEffect(new_effects)
        elif isinstance(norm_effect, UniversalEffect):
            child = norm_effect.effect
            cond_effect = ConditionalEffect(self.condition, child)
            return UniversalEffect(norm_effect.parameters, cond_effect)
        else:
            return ConditionalEffect(self.condition, norm_effect)
    def extract_cost(self):
        return None, self

class UniversalEffect(object):
    def __init__(self, parameters, effect):
        if isinstance(effect, UniversalEffect):
            self.parameters = parameters + effect.parameters
            self.effect = effect.effect
        else:
            self.parameters = parameters
            self.effect = effect
    def dump(self, indent="  "):
        print("%sforall %s" % (indent, ", ".join(map(str, self.parameters))))
        self.effect.dump(indent + "  ")
    def normalize(self):
        norm_effect = self.effect.normalize()
        if isinstance(norm_effect, ConjunctiveEffect):
            new_effects = []
            for effect in norm_effect.effects:
                assert isinstance(effect, SimpleEffect) or isinstance(effect, ConditionalEffect)\
                       or isinstance(effect, UniversalEffect)
                new_effects.append(UniversalEffect(copy.deepcopy(self.parameters), effect))
            return ConjunctiveEffect(new_effects)
        else:
            return UniversalEffect(self.parameters, norm_effect)
    def extract_cost(self):
        return None, self

class ConjunctiveEffect(object):
    def __init__(self, effects):
        flattened_effects = []
        for effect in effects:
            if isinstance(effect, ConjunctiveEffect):
                flattened_effects += effect.effects
            else:
                flattened_effects.append(effect)
        self.effects = flattened_effects
    def dump(self, indent="  "):
        print("%sand" % (indent))
        for eff in self.effects:
            eff.dump(indent + "  ")
    def normalize(self):
        new_effects = []
        for effect in self.effects:
            new_effects.append(effect.normalize())
        return ConjunctiveEffect(new_effects)
    def extract_cost(self):
        new_effects = []
        cost_effect = None
        for effect in self.effects:
            if isinstance(effect, CostEffect):
                cost_effect = effect
            else:
                new_effects.append(effect)
        return cost_effect, ConjunctiveEffect(new_effects)

class SimpleEffect(object):
    def __init__(self, effect):
        self.effect = effect
    def dump(self, indent="  "):
        print("%s%s" % (indent, self.effect))
    def normalize(self):
        return self
    def extract_cost(self):
        return None, self

class CostEffect(object):
    def __init__(self, effect):
        self.effect = effect
    def dump(self, indent="  "):
        print("%s%s" % (indent, self.effect))
    def normalize(self):
        return self
    def extract_cost(self):
        return self, None # this would only happen if
    #an action has no effect apart from the cost effect


#class AssignmentEffect(object):
#    def __init__(self, effect):
#        self.effect = effect
#    def dump(self, indent="  "):
#        print("%s%s" % (indent, self.effect))
#    def normalize(self):
#        return self
#    def extract_cost(self):
#        return self, None


class AssignmentEffect(object):
    def __init__(self, lhs_term, rhs_term):
        self.lhs = lhs_term
        self.rhs = rhs_term
        self.hash = hash((self.__class__, self.lhs, self.rhs))

    def dump(self, indent="  "):
        print("%s%s:=%s" % (indent, self.lhs, self.rhs))
    def normalize(self):
        return self
    def extract_cost(self):
        return self, None

    def __hash__(self):
        return self.hash

    def __eq__(self, other):
        return (hasattr(other, 'hash') and
               self.hash == other.hash and
               self.__class__ is other.__class__ and
               self.lhs == other.lhs_term and
               self.rhs == other.rhs_term)

    def __str__(self):
        return "{self.lhs} := {self.rhs}".format(self=self)

    def get_assigned_symbol(self):
        assert(isinstance(self.lhs, f_expression.FunctionalTerm))
        return self.lhs.symbol

    def rename_variables(self, renamings):
        if len(renamings)== 0 : return self
        self.lhs.rename_variables(renamings)
        self.rhs.rename_variables(renamings)
        # TODO - IMPLEMENT THIS
        #new_args = tuple(renamings.get(arg, arg) for arg in self.args)
        #return self.__class__(self.predicate, new_args)
        return self
