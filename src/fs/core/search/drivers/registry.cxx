
#include <fs/core/problem.hxx>
#include <fs/core/search/drivers/registry.hxx>
// #include <fs/core/search/drivers/gbfs_constrained.hxx>
#include <fs/core/search/drivers/iterated_width.hxx>
#include <fs/core/search/drivers/breadth_first_search.hxx>
#include <fs/core/search/drivers/sbfws/sbfws.hxx>
// #include <fs/core/search/drivers/unreached_atom_driver.hxx>
// #include <fs/core/search/drivers/native_driver.hxx>
#include <fs/core/search/drivers/smart_effect_driver.hxx>
#include <fs/core/search/drivers/smart_lifted_driver.hxx>
#include <fs/core/search/drivers/fully_lifted_driver.hxx>
#include <fs/core/actions/grounding.hxx>


// using namespace fs0::gecode;

namespace fs0 { namespace drivers {



EngineRegistry& EngineRegistry::instance() {
	static EngineRegistry theInstance;
	return theInstance;
}

EngineRegistry::EngineRegistry() {
	// We register the pre-configured search drivers on the instantiation of the singleton
// 	add("standard",  new GBFS_CRPGDriver());
	
// 	add("native",  new NativeDriver<GroundStateModel>());
// 	add("lnative",  new NativeDriver<LiftedStateModel>()); // The native driver is not ready for this
	
// 	add("lunreached",  new UnreachedAtomDriver<LiftedStateModel>());
	add("lifted",  new FullyLiftedDriver());
	
	add("iw",  new IteratedWidthDriver<GroundStateModel>());
	add("liw",  new IteratedWidthDriver<LiftedStateModel>());
	
	add("sbfws",  new bfws::SBFWSDriver<SimpleStateModel>());
	add("lsbfws",  new bfws::SBFWSDriver<LiftedStateModel>());
	
	add("bfs",  new BreadthFirstSearchDriver<GroundStateModel>());
	add("lbfs",  new BreadthFirstSearchDriver<LiftedStateModel>());
	
	add("smart",  new SmartEffectDriver());
	add("lsmart",  new SmartLiftedDriver());
}

EngineRegistry::~EngineRegistry() {
	for (const auto elem:_creators) delete elem.second;
}

void EngineRegistry::add(const std::string& engine_name, Driver* creator) {
auto res = _creators.insert(std::make_pair(engine_name, creator));
	if (!res.second) throw new std::runtime_error("Duplicate registration of engine creator for symbol " + engine_name);
}


Driver* EngineRegistry::get(const std::string& engine_name) {
	auto it = _creators.find(engine_name);
	if (it == _creators.end()) throw std::runtime_error("No engine creator has been registered for given engine name '" + engine_name + "'");
	return it->second;
}


} } // namespaces
