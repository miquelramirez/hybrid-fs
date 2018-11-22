
#include <cassert>
#include <fs/core/utils/projections.hxx>
#include <fs/core/actions/actions.hxx>
#include <fs/core/problem_info.hxx>
#include <fs/core/state.hxx>
#include <fs/core/relaxed_state.hxx>
#include <iostream>

namespace fs0 {

std::vector<object_id> Projections::project(const State& s, const VariableIdxVector& scope) {
	std::vector<object_id> values;
	values.reserve(scope.size());
	for (VariableIdx idx:scope) {
		values.push_back(s.getValue(idx));
	}
	return values;
}


DomainMap Projections::project(RelaxedState& state, const VariableIdxVector& scope) { //!  TODO - CHECK THIS IS APPLYING RVO??
	DomainMap projection;
	for (VariableIdx var:scope) {
		projection.insert(std::make_pair(var, state.getValues(var)));
	}
	return projection;
}

PartialAssignment Projections::zip(const VariableIdxVector& scope, const std::vector<object_id>& values) {
	assert(scope.size() == values.size());
	PartialAssignment assignment;
	for (unsigned i = 0; i < scope.size(); ++i) {
		assignment.insert(std::make_pair(scope[i], values[i]));
	}
	return assignment;
}


const DomainVector Projections::projectValues(const RelaxedState& state, const VariableIdxVector& scope) {
	DomainVector projection;
	for (VariableIdx var:scope) {
		projection.push_back(state.getValues(var));
	}
	return projection;
}


DomainMap Projections::projectCopy(const RelaxedState& state, const VariableIdxVector& scope) {
	DomainMap projection;
	for (VariableIdx var:scope) {
		projection.insert(std::make_pair(var, std::make_shared<Domain>(*(state.getValues(var))))); // We copy construct the whole domain
		assert( *(state.getValues(var)) == *(projection.at(var)) );
	}
	return projection;
}	

DomainVector Projections::project(const DomainMap& domains, const VariableIdxVector& scope) {
	DomainVector projection;
	projection.reserve(scope.size());
	for (VariableIdx var:scope) {
		projection.push_back(domains.at(var));
	}
	return projection;
}

DomainMap Projections::clone(const DomainMap& domains) {
	DomainMap clone;
	for (const auto& domain:domains) {
		clone.insert(std::make_pair(domain.first, std::make_shared<Domain>(*(domain.second))));
	}
	return clone;
}

void Projections::printDomains(const DomainMap& domains) {
	const ProblemInfo& problemInfo = ProblemInfo::getInstance();
	for (const auto& domain:domains) {
		std::cout << problemInfo.getVariableName(domain.first) << "={";
		for (auto obj:*(domain.second)) {
			std::cout << problemInfo.object_name(obj) << ",";
		}
		std::cout << "}" << std::endl;
	}
}

void Projections::printDomains(const DomainVector& domains) {
	for (unsigned i = 0; i < domains.size(); ++i) {
		std::cout << "variable #" << i << "=";
		printDomain(*domains[i]);
		std::cout << std::endl;
	}
}

void Projections::printDomain(const Domain& domain) {
	std::cout << "{";
	for (auto objIdx:domain) {
		std::cout << objIdx << ",";
	}
	std::cout << "}";
	std::cout.flush();
}

} // namespaces
