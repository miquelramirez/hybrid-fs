
# FS Beta7 Refactoring

## Code Layout

* All of the source/header files which are not part of the future `hybrid` module
  now lie under `src/fs/core`.

* The files corresponding to the hybrid module now lie under `src/fs/hybrid`.

* The _rapidjson_ library is no longer under the `src` directory, but rather
  on a first-level `vendor` directory (`lib` was already taken!).
  Additionally, the code is no longer versioned on our repo, but instead we fetch it using submodules.
  This means that you need to `git submodule update` whenever a change in the rapidjson library
  is factored into the main FS project - which shouldn't happen too often.
  The `vendor` directory is meant to be part of the include paths of the build system, so that we only need to e.g.
  `#include <rapidjson/xxx.hxx>`. That's good if we want to eventually transition to a system-wide installation, etc.

* The SConscript's files, scons utils, etc., _except for the main `SConstruct` file_ are now under top-level directory `build`.
  This includes the python helpers that were before in the top-level `utils` directory.
  We might want to move the `SConstruct` as well? It was too much of delicate renaming work for now though.

* The libraries are now generated by scons under the `.build` directory, the same place where the `.o` code is output.

## Other issues
* All include directives `#include <fs/core/...>` should be absolute now.