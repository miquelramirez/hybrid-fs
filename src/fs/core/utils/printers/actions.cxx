
#include <fs/core/problem.hxx>
#include <fs/core/languages/fstrips/language.hxx>
#include <fs/core/fstrips/language.hxx>
#include <fs/core/utils/printers/actions.hxx>
#include <fs/core/utils/printers/binding.hxx>
#include <fs/core/actions/actions.hxx>


namespace fs0 { namespace print {

std::ostream& action_data_name::print(std::ostream& os) const {
	os << _action.getName() << "(" << print::signature(_action.getParameterNames(), _action.getSignature()) << ")";
	return os;
}


std::ostream& action_signature::print(std::ostream& os) const {
	os << _action.getName() << "(" << print::signature(_action.getParameterNames(), _action.getSignature()) << ")";
	return os;
}


std::ostream& action_data::print(std::ostream& os) const {
	os << action_data_name(_action) << std::endl;
	os << "\t" << "Type: ";
	if (_action.getType() == ActionData::Type::Control )
		os << "control" << std::endl;
	else if (_action.getType() == ActionData::Type::Exogenous )
		os << "exogenous" << std::endl;
	else if (_action.getType() == ActionData::Type::Natural )
        	os << "natural" << std::endl;
	os << "\t" << "Precondition: " << *_action.getPrecondition() << std::endl;
	os << "\t" << "Effects:" << std::endl;
	for (auto elem:_action.getEffects()) os << "\t\t" << *elem << std::endl;
	return os;
}

std::ostream& action_header::print(std::ostream& os) const {
	os << _action.getName() << "(" << print::partial_binding(_action.getParameterNames(), _action.getBinding(), _action.getSignature()) << ")";
	return os;
}

std::ostream& strips_action_header::print(std::ostream& os) const {
	os << "(" << _action.getName() << " " << print::strips_partial_binding(_action.getParameterNames(), _action.getBinding(), _action.getSignature()) << ")";
	return os;
}

std::ostream& full_action::print(std::ostream& os) const {
	os << action_header(_action) << std::endl;
	os << "\t" << "Precondition:" << *_action.getPrecondition() << std::endl;
	os << "\t" << "Effects:" << std::endl;
	for (auto elem:_action.getEffects()) os << "\t\t" << *elem << std::endl;
	return os;
}

std::ostream& actions::print(std::ostream& os) const {
	for (const GroundAction* action:_actions) {
		os << action->getId() << ": " << action_header(*action) << std::endl;
	}
	return os;
}

} } // namespaces
