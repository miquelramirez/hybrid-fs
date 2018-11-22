
#include <fs/core/problem_info.hxx>
#include <fs/core/constraints/direct/compiled.hxx>
#include <fs/core/utils/projections.hxx>
#include <fs/core/utils/utils.hxx>
#include <fs/core/utils/printers/vector.hxx>
#include <fs/core/utils/printers/helper.hxx>
#include <fs/core/languages/fstrips/scopes.hxx>
#include <fs/core/languages/fstrips/language.hxx>

namespace fs0 {

CompiledUnaryConstraint::CompiledUnaryConstraint(const VariableIdxVector& scope, const ValueTuple& parameters, ExtensionT&& extension) : 
	UnaryDirectConstraint(scope, parameters), _extension(extension)
{}

CompiledUnaryConstraint::CompiledUnaryConstraint(const UnaryDirectConstraint& constraint) :
	CompiledUnaryConstraint(constraint.getScope(), constraint.getParameters(), _compile(constraint))
{}

CompiledUnaryConstraint::CompiledUnaryConstraint(const VariableIdxVector& scope, const Tester& tester) 
	: CompiledUnaryConstraint(scope, {}, _compile(scope, tester))
{}

std::set<CompiledUnaryConstraint::ElementT> CompiledUnaryConstraint::compile(const VariableIdxVector& scope, const CompiledUnaryConstraint::Tester& tester) {
	const ProblemInfo& info = ProblemInfo::getInstance();
	assert(scope.size() == 1);
	
	std::set<ElementT> ordered;
	for(const object_id& value:info.getVariableObjects(scope[0])) {
		if (tester(value)) ordered.insert(value);
	}
	return ordered;
}


CompiledUnaryConstraint::ExtensionT CompiledUnaryConstraint::_compile(const VariableIdxVector& scope, const Tester& tester) {
	auto ordered = compile(scope, tester);
	return ExtensionT(ordered.begin(), ordered.end());
}


CompiledUnaryConstraint::ExtensionT CompiledUnaryConstraint::_compile(const UnaryDirectConstraint& constraint) {
	auto ordered = compile(constraint);
	return ExtensionT(ordered.begin(), ordered.end());
}

bool CompiledUnaryConstraint::isSatisfied(const object_id& o) const {
	return std::binary_search(_extension.begin(), _extension.end(), o); // TODO - Change for a O(1) lookup in a std::unordered_set ?
}

FilteringOutput CompiledUnaryConstraint::filter(const DomainMap& domains) const {
	
	DomainVector projection = Projections::project(domains, _scope);
	assert(projection.size() == 1);
	Domain& domain = *(projection[0]);
	Domain new_domain;

	std::set_intersection(domain.begin(), domain.end(), _extension.begin(), _extension.end(), std::inserter(new_domain, new_domain.end()));
	
	if (new_domain.size() == domain.size()) return FilteringOutput::Unpruned;
	if (new_domain.size() == 0) return FilteringOutput::Failure;
	
	// Otherwise the domain has necessarily been pruned
	domain = new_domain;  // Update the domain with the new values using the assignment operator
	return FilteringOutput::Pruned;
}

std::ostream& CompiledUnaryConstraint::print(std::ostream& os) const {
	const ProblemInfo& info = ProblemInfo::getInstance();
	os << "CompiledUnaryConstraint[" << info.getVariableName(_scope[0]) << "] = {";
	for (const auto& elem:_extension) {
		os << elem;
	}
	os << "}";
	return os;
}

CompiledBinaryConstraint::CompiledBinaryConstraint(const BinaryDirectConstraint& constraint, const ProblemInfo& problemInfo) :
	CompiledBinaryConstraint(constraint.getScope(), constraint.getParameters(), compile(constraint))
{}

CompiledBinaryConstraint::CompiledBinaryConstraint(const VariableIdxVector& scope, const ValueTuple& parameters, const CompiledBinaryConstraint::TupleExtension& extension) 
	: BinaryDirectConstraint(scope, parameters), _extension1(index(extension, 0)),  _extension2(index(extension, 1))
{}

CompiledBinaryConstraint::CompiledBinaryConstraint(const VariableIdxVector& scope, const CompiledBinaryConstraint::Tester& tester) 
	: CompiledBinaryConstraint(scope, {}, compile(scope, tester))
{}


bool CompiledBinaryConstraint::isSatisfied(const object_id& o1, const object_id& o2) const {
	auto iter = _extension1.find(o1);
	assert(iter != _extension1.end());
	const std::vector<object_id>& D_y = iter->second; // iter->second contains all the elements y of the domain of the second variable such that <x, y> satisfies the constraint
	return std::binary_search(D_y.begin(), D_y.end(), o2); // TODO - Change for a O(1) lookup in a std::unordered_set ?
}

CompiledBinaryConstraint::TupleExtension CompiledBinaryConstraint::compile(const VariableIdxVector& scope, const CompiledBinaryConstraint::Tester& tester) {
	assert(scope.size()==2);
	const ProblemInfo& info = ProblemInfo::getInstance();
	
	TupleExtension extension;
	
	for(const object_id& x:info.getVariableObjects(scope[0])) {
		for(const object_id& y:info.getVariableObjects(scope[1])) {
			if (tester(x, y)) {
				extension.insert(std::make_tuple(x, y));
			}
		}
	}
	return extension;
}


CompiledBinaryConstraint::ExtensionT CompiledBinaryConstraint::index(const CompiledBinaryConstraint::TupleExtension& extension, unsigned variable) {
	assert(variable == 0 || variable == 1);
	std::map<object_id, std::set<object_id>> values;
	
	// We perform two passes to make sure that elements are sorted, although depending on the compilation flags this is unnecessary
	
	for (const auto& tuple:extension) {
		object_id x = (variable == 0) ? std::get<0>(tuple) : std::get<1>(tuple);
		object_id y = (variable == 0) ? std::get<1>(tuple) : std::get<0>(tuple);
		values[x].insert(y);
	}

	ExtensionT res;
	for (const auto& element:values) {
		res.insert(std::make_pair(element.first, std::vector<object_id>(element.second.begin(), element.second.end()) ));
	}
	return res;
}


FilteringOutput CompiledBinaryConstraint::filter(unsigned variable) const {
	assert(projection.size() == 2);
	assert(variable == 0 || variable == 1);
	unsigned other = (variable == 0) ? 1 : 0;
	const ExtensionT& extension_map = (variable == 0) ? _extension1 : _extension2;
	
	Domain& domain = *(projection[variable]);
	Domain& other_domain = *(projection[other]);
	Domain new_domain;
	
	for (const object_id&
 x:domain) {
		auto iter = extension_map.find(x);
		assert(iter != extension_map.end());
		const auto& D_y = iter->second; // iter->second contains all the elements y of the domain of the second variable such that <x, y> satisfies the constraint
		if (!Utils::empty_intersection(other_domain.begin(), other_domain.end(), D_y.begin(), D_y.end())) {
			new_domain.insert(new_domain.cend(), x); //  x is an arc-consistent value. We will insert on the end of the container, as it is already sorted.
		}
	}
	if (new_domain.size() == domain.size()) return FilteringOutput::Unpruned;
	if (new_domain.size() == 0) return FilteringOutput::Failure;

	// Otherwise the domain has necessarily been pruned
	domain = new_domain; // Update the domain by using the assignment operator.
	return FilteringOutput::Pruned;
}

std::ostream& CompiledBinaryConstraint::print(std::ostream& os) const {
	os << "CompiledBinaryConstraint[" << print::container(print::Helper::name_variables(_scope)) << "] = {" << std::endl;
	os << "First view: " << std::endl;
	for (const auto& elem:_extension1) {
		os << "\t" << elem.first << ": [";
		for (const auto& elem2:elem.second) {
			os << elem2 << ", ";
		}
		os << "]" << std::endl;
	}
	
	os << "Second view: " << std::endl;
	for (const auto& elem:_extension2) {
	os << "\t" << elem.first << ": [";
	for (const auto& elem2:elem.second) {
		os << elem2 << ", ";
	}
	os << "]" << std::endl;
	}
	os << "}";
	return os;
}

CompiledUnaryEffect::CompiledUnaryEffect(VariableIdx relevant, VariableIdx affected, ExtensionT&& extension)
	: UnaryDirectEffect(relevant, affected, {}), _extension(extension)
{}
	
CompiledUnaryEffect::CompiledUnaryEffect(VariableIdx relevant, VariableIdx affected, const fs::Term& term) 
	: CompiledUnaryEffect(relevant, affected, compile(term, ProblemInfo::getInstance()))
{}
	

Atom CompiledUnaryEffect::apply(const object_id& value) const {
	return Atom(_affected, _extension.at(value));
}

CompiledUnaryEffect::ExtensionT CompiledUnaryEffect::compile(const UnaryDirectEffect& effect, const fs0::ProblemInfo& info) {
	VariableIdx relevant = effect.getScope()[0];
	const std::vector<object_id>& all_values = info.getVariableObjects(relevant);
	
	ExtensionT map;
	for(const object_id& value:all_values) {
		try {
			auto atom = effect.apply(value);
			assert(atom.getVariable() == effect.getAffected());
			map.insert(std::make_pair(value, atom.getValue()));
		} catch(const std::out_of_range& e) {}  // If the effect produces an exception, we simply consider it non-applicable and go on.
	}
	return map;
}

CompiledUnaryEffect::ExtensionT CompiledUnaryEffect::compile(const fs::Term& term, const ProblemInfo& info) {
	VariableIdxVector scope = fs::ScopeUtils::computeDirectScope(&term);
	assert(scope.size() == 1);
	ExtensionT map;
	
	for(const object_id& value:info.getVariableObjects(scope[0])) {
		try {
			object_id out = term.interpret(Projections::zip(scope, {value}));
			map.insert(std::make_pair(value, out));
		} catch(const std::out_of_range& e) {}  // If the effect produces an exception, we simply consider it non-applicable and go on.
	}
	return map;
}

CompiledBinaryEffect::CompiledBinaryEffect(const VariableIdxVector& scope, VariableIdx affected, ExtensionT&& extension) 
	: BinaryDirectEffect(scope, affected, {}), _extension(extension)
{}

CompiledBinaryEffect::CompiledBinaryEffect(const VariableIdxVector& scope, VariableIdx affected, const fs::Term& term)
	: CompiledBinaryEffect(scope, affected, compile(term, ProblemInfo::getInstance()))
{}

CompiledBinaryEffect::ExtensionT CompiledBinaryEffect::compile(const fs::Term& term, const ProblemInfo& info) {
	VariableIdxVector scope = fs::ScopeUtils::computeDirectScope(&term);
	assert(scope.size() == 2);
	ExtensionT map;
	
	for(const object_id& x:info.getVariableObjects(scope[0])) {
		for(const object_id& y:info.getVariableObjects(scope[1])) {
			try {
				object_id out = term.interpret(Projections::zip(scope, {x, y}));
				map.insert(std::make_pair(std::make_pair(x, y), out));
			} catch(const std::out_of_range& e) {}  // If the effect produces an exception, we simply consider it non-applicable and go on.
		}
	}
	return map;
}


Atom CompiledBinaryEffect::apply(const object_id& v1, const object_id& v2) const {
	return Atom(_affected, _extension.at({v1, v2}));
}


unsigned ConstraintCompiler::compileConstraints(std::vector<DirectConstraint*>& constraints) {
	const ProblemInfo& info = ProblemInfo::getInstance();
	
	unsigned num_compiled = 0;
	for (unsigned i = 0; i < constraints.size(); ++i) {
		DirectConstraint* compiled = constraints[i]->compile(info);
		if (compiled) { // The constraint type requires pre-compilation
			delete constraints[i];
			constraints[i] = compiled;
			++num_compiled;
		}
	}
	return num_compiled;	
}


} // namespaces
