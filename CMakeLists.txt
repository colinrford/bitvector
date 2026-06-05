cmake_minimum_required(VERSION 3.31.6 FATAL_ERROR)

if(CMAKE_VERSION VERSION_GREATER_EQUAL 4.3.0)
  set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "451f2fe2-a8a2-47c3-bc32-94786d8fc91b")
elseif(CMAKE_VERSION VERSION_GREATER_EQUAL 4.0.3)
  set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
elseif(CMAKE_VERSION VERSION_GREATER_EQUAL 4.0.0)
  set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "a9e1cf81-9932-4810-974b-6eccaf14e457")
elseif(CMAKE_VERSION VERSION_GREATER_EQUAL 3.31.6)
  set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "0e5b6991-d74f-4b3d-a41c-cf096e0b2508")
else()
  message(FATAL_ERROR "Version lower than 3.31.6 not supported (by me)")
endif()

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_MODULE_STD ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

## PROJECT
## name and version — VERSION file is the single source of truth (CMake + Conan).
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" _lam_bitvector_version)
string(STRIP "${_lam_bitvector_version}" _lam_bitvector_version)
project(lam_bitvector VERSION ${_lam_bitvector_version} LANGUAGES CXX)

# Re-run CMake configure whenever VERSION changes.
set_property(DIRECTORY APPEND PROPERTY
  CMAKE_CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# lam build helpers
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(Sanitizers)
include(LamConstexprLimit)
# lam_add_config_partition / lam_add_config_dump / lam_install_module_package
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/LamModule.cmake)
# bitvector_add_executable() for the tests/ benchmarks/ examples/ dev targets
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/BitvectorHelpers.cmake)

## OPTIONS
# Dev targets, ON for normal source builds; the Conan recipe passes all three
# OFF because tests/ benchmarks/ examples/ aren't in exports_sources. Examples
# degrade gracefully: ctbignum_interop self-skips when lam.ctbignum isn't found.
option(LAM_BITVECTOR_BUILD_TESTS      "Build the unit tests." ON)
option(LAM_BITVECTOR_BUILD_BENCHMARKS "Build the benchmarks." ON)
option(LAM_BITVECTOR_BUILD_EXAMPLES   "Build the examples (ctbignum_interop self-skips if lam.ctbignum absent)." ON)

## TARGET
add_library(bitvector)
add_library(lam::bitvector ALIAS bitvector)
add_library(lam_bitvector::bitvector ALIAS bitvector)
# Target name stays `bitvector`; OUTPUT_NAME/EXPORT_NAME give the installed
# package its lam_bitvector::bitvector target, matching the other lam submodules.
set_target_properties(bitvector PROPERTIES
  OUTPUT_NAME lam_bitvector
  EXPORT_NAME bitvector)
target_sources(bitvector
  PUBLIC
  FILE_SET CXX_MODULES
  TYPE CXX_MODULES
  BASE_DIRS ${PROJECT_SOURCE_DIR}/src/cpp
  FILES src/cpp/bitvector.cppm
        src/cpp/bitvector-core.cppm
        src/cpp/bitvector-boolean_ops.cppm
        src/cpp/bitvector-algorithms.cppm
)

target_compile_features(bitvector PUBLIC cxx_std_23)

# Generate + attach the lam.bitvector:config partition (build-time git_hash refresh).
lam_add_config_partition(bitvector NAME bitvector)

enable_testing()

# config_dump smoke test + build-config diagnostic (imports lam.bitvector).
# NAMESPACE override: the config partition lives in lam::bitvec, not lam::bitvector.
lam_add_config_dump(bitvector NAME bitvector NAMESPACE lam::bitvec::config)

## TESTS / BENCHMARKS / EXAMPLES
if(LAM_BITVECTOR_BUILD_TESTS)
  add_subdirectory(tests)
endif()

if(LAM_BITVECTOR_BUILD_BENCHMARKS)
  add_subdirectory(benchmarks)
endif()

if(LAM_BITVECTOR_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

## INSTALL
## Hand-rolled install(TARGETS/EXPORT) + legacy Config.cmake. CPS (PACKAGE_INFO)
## and per-submodule licensing are intentionally deferred here — when they land,
## this block can move to lam_install_module_package() like the other modules.
include(GNUInstallDirs)

install(TARGETS bitvector
  EXPORT bitvectorTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  FILE_SET CXX_MODULES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/lam/bitvector
)

install(EXPORT bitvectorTargets
  FILE lam_bitvectorTargets.cmake
  NAMESPACE lam_bitvector::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/lam_bitvector
)

include(CMakePackageConfigHelpers)

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/lam_bitvectorConfig.cmake" [[
include(CMakeFindDependencyMacro)
include("${CMAKE_CURRENT_LIST_DIR}/lam_bitvectorTargets.cmake")
]])

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/lam_bitvectorConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/lam_bitvectorConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/lam_bitvectorConfigVersion.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/lam_bitvector
)
