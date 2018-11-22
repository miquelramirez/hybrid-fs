
#pragma once

#include <fs/core/search/algorithms/ehc.hxx>

#include <fs/core/search/drivers/registry.hxx>
#include <fs/core/state.hxx>
#include <fs/core/problem.hxx>

namespace fs0 { namespace drivers {


//! A combined search strategy that first applies a given Enhanced Hill-Climbing and then, if the goal was not found,
//! a standard GBFS.
template <typename SearchAlgorithmT, typename HeuristicT>
class EHCThenGBFSSearch {
public:
	~EHCThenGBFSSearch() = default;
	EHCThenGBFSSearch(const EHCThenGBFSSearch&) = default;
	EHCThenGBFSSearch(EHCThenGBFSSearch&&) = default;
	EHCThenGBFSSearch& operator=(const EHCThenGBFSSearch&) = default;
	EHCThenGBFSSearch& operator=(EHCThenGBFSSearch&&) = default;

	EHCThenGBFSSearch(const Problem& problem, SearchAlgorithmT* gbfs, EHCSearch<HeuristicT>* ehc) :
		_problem(problem), _gbfs(gbfs), _ehc(ehc)
	{}

	bool search(const State& state, std::vector<unsigned>& solution) {
		assert(solution.size()==0);
		bool result = false;

		if (_ehc) {
			result = _ehc->search(state, solution);
			if (result) {
				LPT_INFO("search", "Solution found in EHC phase");
				return true;
			}

			// If EHC found no solution, then we switch to a standard GBFS.
			LPT_INFO("search", "No solution found in EHC phase, switching to a GBFS");
			solution.clear();
		}

		result = _gbfs->search(state, solution);
		return result;
	}

	//! Convenience method
	bool solve_model(std::vector<unsigned>& solution) { return search( _problem.getInitialState(), solution ); }

protected:

	//!
	const Problem& _problem;

	//!
	std::unique_ptr<SearchAlgorithmT> _gbfs;

	//!
	std::unique_ptr<EHCSearch<HeuristicT>> _ehc;
};

} } // namespaces
