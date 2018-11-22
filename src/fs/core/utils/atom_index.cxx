
#include <iostream>
#include <boost/functional/hash.hpp>

#include <fs/core/utils/atom_index.hxx>
#include <fs/core/problem_info.hxx>
#include <lapkt/tools/logging.hxx>

namespace fs0 {

bool AtomIndex::is_indexed(VariableIdx variable, const object_id& value) const {
	return !_info.isPredicativeVariable(variable) || _indexes_negated_literals || int(value) == 1;
}


AtomIndex::AtomIndex(const ProblemInfo& info, bool index_negated_literals) :
	_info(info),
	_indexes_negated_literals(index_negated_literals),
	_tuple_index_inv(info.getNumLogicalSymbols()),
	_atom_index_inv(info.getNumVariables()),
	_variable_to_atom_index(info.getNumVariables())
{
	auto tuples_by_symbol = compute_all_reachable_tuples(info);

	std::vector<std::pair<unsigned, unsigned>> symbol_ranges;
	unsigned idx = 0;
	for (unsigned symbol = 0; symbol < info.getNumLogicalSymbols(); ++symbol) {
		std::pair<unsigned, unsigned> range{idx, 0};

		for (const std::pair<ValueTuple, object_id>& data:tuples_by_symbol.at(symbol)) {
			// For each symbol, we process all derived tuples and keep information
			// about the index range of the tuples that correspond to that symbol.

			const ValueTuple& arguments = data.first;
			const object_id& value = data.second;

			ValueTuple tuple = arguments; // Copy the vector
			if (info.isFunction(symbol)) { // A functional symbol, we want to store the arguments+value tuple
				tuple.push_back(value);
			}

			if (_indexes_negated_literals || info.isFunction(symbol) || value) {
				VariableIdx variable = info.resolveStateVariable(symbol, arguments);
				add(info, symbol, tuple, idx, Atom(variable, value));
				idx++;
			}
		}

		range.second = idx - 1;
		symbol_ranges.push_back(range);
	}
}

void AtomIndex::add(const ProblemInfo& info, unsigned symbol, const ValueTuple& tuple, unsigned idx, const Atom& atom) {
	assert(_tuple_index.size() == idx);
	_tuple_index.push_back(tuple);

	assert(_symbol_index.size() == idx);
	_symbol_index.push_back(symbol);

	if (info.isFunction(symbol) || int(atom.getValue()) == 1) { // For predicative symbols, we only map the logical symbol to the index of the corresponding "true" atom
		_tuple_index_inv.at(symbol).insert(std::make_pair(tuple, idx));
	}

	assert(_atom_index.size() == idx);
	_atom_index.push_back(atom);

	_atom_index_inv.at(atom.getVariable()).insert(std::make_pair(atom.getValue(), idx));
	_variable_to_atom_index.at(atom.getVariable()).push_back(idx);
}

AtomIdx AtomIndex::to_index(unsigned symbol, const ValueTuple& tuple) const {
	const auto& map = _tuple_index_inv.at(symbol);
	auto it = map.find(tuple);
	assert(it != map.end());
	return it->second;
}

AtomIdx AtomIndex::to_index(const Atom& atom) const {
	return to_index(atom.getVariable(), atom.getValue());
}

AtomIdx AtomIndex::to_index(VariableIdx variable, const object_id& value) const {
	const auto& map = _atom_index_inv.at(variable);
	auto it = map.find(value);
	assert(it != map.end());
	return it->second;
}

// TODO - We should be applying some reachability analysis here to prune out tuples that will never be reachable at all.
std::vector<std::vector<std::pair<ValueTuple, object_id>>> AtomIndex::compute_all_reachable_tuples(const ProblemInfo& info) {
	std::vector<std::vector<std::pair<ValueTuple, object_id>>> tuples_by_symbol(info.getNumLogicalSymbols());

	for (VariableIdx var = 0; var < info.getNumVariables(); ++var) {
		const auto& data = info.getVariableData(var);
		auto& symbol_tuples = tuples_by_symbol.at(data.first); // The tupleset corresponding to the symbol index
		LPT_DEBUG("atom_index", "# Objects in Domain of Variable (#'" << var << "') '" << info.getVariableName(var)<< "' = " << info.getVariableObjects(var).size())
		for (const object_id& value:info.getVariableObjects(var)) {
			symbol_tuples.push_back(std::make_pair(data.second, value));
		}
	}
	return tuples_by_symbol;
}

} // namespaces
