
#include <fs/core/actions/action_id.hxx>
#include <fs/core/actions/actions.hxx>
#include <fs/core/actions/grounding.hxx>
#include <fs/core/problem_info.hxx>
#include <fs/core/utils/printers/actions.hxx>
#include <boost/functional/hash.hpp>


namespace fs0 {

const LiftedActionID LiftedActionID::invalid_action_id = LiftedActionID(nullptr, std::vector<object_id>());


LiftedActionID::LiftedActionID(const PartiallyGroundedAction* action, Binding&& binding)
	: _action(action), _binding(std::move(binding)), _hash(0), _hashed(false)
{
	// The binding of the PartiallyGroundedAction might be incomplete, but the binding of the LiftedActionID must be complete.
	assert(_action == nullptr || get_full_binding().is_complete());
}

bool LiftedActionID::operator==(const ActionID& rhs) const {
	// Two lifted actions are equal iff they arise from the same action schema and have the same full binding
	auto derived = dynamic_cast<const LiftedActionID*>(&rhs);
	if(!derived) return false;
	if (!_action  || !derived->_action) return (!_action  && !derived->_action); // If one is the null ptr, both must be null to be equal!
	if (hash() != rhs.hash()) return false; // For faster non-equality detection
	return _action->getOriginId() == derived->_action->getOriginId() && _binding == derived->_binding;
}

unsigned PlainActionID::id() const { return _action->getId(); }

bool PlainActionID::operator==(const ActionID& rhs) const {
	auto derived = dynamic_cast<const PlainActionID*>(&rhs);
	if(!derived) return false;
	return  id() == derived->id();
}

std::size_t LiftedActionID::hash() const {
	if (!_hashed) {
		_hash = generate_hash();
		_hashed = true;
	}
	return _hash;
}

std::size_t LiftedActionID::generate_hash() const {
	std::size_t hash = 0;
	const Binding binding = get_full_binding();
	const std::vector<object_id>& binding_data = binding.get_full_binding(); // TODO This is rather suboptimal
	boost::hash_combine(hash, typeid(*this).hash_code());
	boost::hash_combine(hash, _action->getOriginId());
	boost::hash_combine(hash, boost::hash_range(binding_data.begin(), binding_data.end()));
	return hash;
}

std::size_t PlainActionID::hash() const {
	std::size_t hash = 0;
	boost::hash_combine(hash, typeid(*this).hash_code());
	boost::hash_combine(hash, id());
	return hash;
}

std::ostream& LiftedActionID::print(std::ostream& os) const {
	if (!_action) {
		return os << "INVALID-ACTION";
	}
	
	std::unique_ptr<const GroundAction> ground(generate());
	return os << *ground;
}

std::ostream& PlainActionID::print(std::ostream& os) const {
	(_action? os << *_action : os << "INVALID-ACTION");
	return os;
}

GroundAction* LiftedActionID::generate() const {
	const ProblemInfo& info = ProblemInfo::getInstance();
	return ActionGrounder::bind(*_action, _binding, info);
}

Binding LiftedActionID::get_full_binding() const {
	Binding full(_action->getBinding());
	full.merge_with(_binding);
	return full;
}

} // namespaces
