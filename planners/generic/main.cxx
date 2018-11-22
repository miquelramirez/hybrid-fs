
#include <fs/core/search/options.hxx>
#include <fs/core/search/runner.hxx>
#include <fs/core/utils/system.hxx>

// This include will dinamically point to the adequate per-instance automatically generated file
#include <components.hxx>  


// Our main function simply creates a runner with the command-line options plus the generator function
// which is specific to a single problem instance, and then runs the search engine
int main(int argc, char** argv) {
	fs0::init_fs_system();
	fs0::drivers::Runner runner(fs0::drivers::EngineOptions(argc, argv), generate);
	return runner.run();
}
