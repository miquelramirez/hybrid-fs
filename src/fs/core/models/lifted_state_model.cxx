
#include <fs/core/models/lifted_state_model.hxx>
#include <fs/core/problem.hxx>
#include <fs/core/state.hxx>
#include <fs/core/applicability/formula_interpreter.hxx>
#include <fs/core/applicability/action_managers.hxx>
#include <fs/core/actions/actions.hxx>

#include <fs/core/languages/fstrips/language.hxx>
#include <fs/core/constraints/gecode/handlers/lifted_action_csp.hxx>


//! A helper to derive the distinct goal atoms
std::vector<const fs::Formula*>
obtain_goal_atoms(const fs::Formula* goal) {
	std::vector<const fs::Formula*> goal_atoms;

	const fs::Conjunction* conjunction = dynamic_cast<const fs::Conjunction*>(goal);
	if (!conjunction) {
		goal_atoms.push_back(goal);
	} else {

		for (const fs::Formula* atom:conjunction->getSubformulae()) {
			goal_atoms.push_back(atom);
		}

	}

	return goal_atoms;
}


namespace fs0 {

State LiftedStateModel::init() const {
	// We need to make a copy so that we can return it as non-const.
	// Ugly, but this way we make it fit the search engine interface without further changes,
	// and this is only called once per search.
	return State(_task.getInitialState());
}

bool LiftedStateModel::goal(const State& state) const {
	return _task.getGoalSatManager().satisfied(state);
}

bool LiftedStateModel::is_applicable(const State& state, const ActionType& action, bool enforce_state_constraints) const {
	auto ground_action = action.generate();
	bool res = is_applicable(state, *ground_action, enforce_state_constraints);
	delete ground_action;
	return res;
}

bool LiftedStateModel::is_applicable(const State& state, const GroundAction& action, bool enforce_state_constraints) const {
	NaiveApplicabilityManager manager(_task.getStateConstraints());
	return manager.isApplicable(state, action, enforce_state_constraints);
}

State LiftedStateModel::next(const State& state, const LiftedActionID& action) const {
	auto ground_action = action.generate();
	auto s1 = next(state, *ground_action);
	delete ground_action;
	return s1;
}

State LiftedStateModel::next(const State& state, const GroundAction& action) const {
	NaiveApplicabilityManager manager(_task.getStateConstraints());
	action.apply( state, _effects_cache );
	return State(state, _effects_cache); // Copy everything into the new state and apply the changeset
}


gecode::LiftedActionIterator LiftedStateModel::applicable_actions(const State& state, bool enforce_state_constraints) const {
	if ( enforce_state_constraints )
		return gecode::LiftedActionIterator(state, _handlers, _task.getStateConstraints());
	return gecode::LiftedActionIterator(state, _handlers, {});
}


bool
LiftedStateModel::goal(const StateT& s, unsigned i) const {
	Binding binding;
	return _subgoals.at(i)->interpret(s, binding);
// 	return s.contains(_subgoals.at(i)); // TODO SHOULD BE:
	// const Atom& subgoal = _subgoals.at(i);
	// return s.check(subgoal.getVariable(), s.getValue());
}

LiftedStateModel
LiftedStateModel::build(const Problem& problem) {
	auto model = LiftedStateModel(problem, obtain_goal_atoms(problem.getGoalConditions()));
	model.set_handlers(gecode::LiftedActionCSP::create_derived(problem.getPartiallyGroundedActions(), problem.get_tuple_index(), false, false));
	return model;
}

LiftedStateModel::LiftedStateModel(const Problem& problem, const std::vector<const fs::Formula*>& subgoals) :
	_task(problem),
	_subgoals(subgoals)
{}

} // namespaces
