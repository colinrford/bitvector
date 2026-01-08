cmake_minimum_required(VERSION 3.31.6 FATAL_ERROR)

if(CMAKE_VERSION VERSION_GREATER_EQUAL 4.0.3)
  set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
elseif(CMAKE_VERSION VERSION_GREATER_EQUAL 4.0.0)
  set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "a9e1cf81-9932-4810-974b-6eccaf14e457")
elseif(CMAKE_VERSION VERSION_GREATER_EQUAL 3.31.6)
  set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "0e5b6991-d74f-4b3d-a41c-cf096e0b2508")
else()
  message(FATAL_ERROR "Version lower than 3.31.6 not supported (by me)")
endif()

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED True) #is this needed?
set(CMAKE_CXX_MODULE_STD ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

project(lam_bitvector VERSION 0.2.260107 LANGUAGES CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(Git)
if(GIT_FOUND)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
else()
  set(GIT_HASH "unknown")
endif()

if(CMAKE_BUILD_TYPE AND CMAKE_BUILD_TYPE MATCHES "^[Dd][Ee][Bb][Uu][Gg]$")
  set(IS_DEBUG "true")
else()
  set(IS_DEBUG "false")
endif()

configure_file(
  config.cppm.in
  ${CMAKE_CURRENT_BINARY_DIR}/config.cppm
)

# Sanitizer Options (all OFF by default for performance)
option(LAM_ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(LAM_ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)
option(LAM_ENABLE_TSAN "Enable ThreadSanitizer (cannot combine with ASan)" OFF)

if(LAM_ENABLE_TSAN AND LAM_ENABLE_ASAN)
  message(FATAL_ERROR "Cannot enable both TSan and ASan simultaneously")
endif()

if(LAM_ENABLE_ASAN)
  add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address)
endif()

if(LAM_ENABLE_UBSAN)
  add_compile_options(-fsanitize=undefined -fno-omit-frame-pointer)
  add_link_options(-fsanitize=undefined)
endif()

if(LAM_ENABLE_TSAN)
  add_compile_options(-fsanitize=thread -fno-omit-frame-pointer)
  add_link_options(-fsanitize=thread)
endif()

add_library(bitvector)
add_library(lam::bitvector ALIAS bitvector)
set_target_properties(bitvector PROPERTIES OUTPUT_NAME "lam_bitvector")
target_sources(bitvector
  PUBLIC
  FILE_SET CXX_MODULES
  TYPE CXX_MODULES
  FILES src/cpp/bitvector.cppm
        src/cpp/bitvector-core.cppm
        src/cpp/bitvector-boolean_ops.cppm
        src/cpp/bitvector-algorithms.cppm
        ${CMAKE_CURRENT_BINARY_DIR}/config.cppm
)

target_compile_features(bitvector PUBLIC cxx_std_23)


# Installation Configuration
include(GNUInstallDirs)

# Library Installation
install(TARGETS bitvector
  EXPORT bitvectorTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  FILE_SET CXX_MODULES DESTINATION ${CMAKE_INSTALL_LIBDIR}/c++/v1
)

install(EXPORT bitvectorTargets
  FILE bitvectorTargets.cmake
  NAMESPACE lam::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/lam_bitvector
)

include(CMakePackageConfigHelpers)

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/lam_bitvectorConfig.cmake" [[
include(CMakeFindDependencyMacro)
include("${CMAKE_CURRENT_LIST_DIR}/bitvectorTargets.cmake")
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

# Benchmark Configuration
add_subdirectory(benchmarks)

enable_testing()
add_subdirectory(tests)

# Optional Integration Examples
find_package(lam_ctbignum CONFIG QUIET)
if(lam_ctbignum_FOUND)
  message(STATUS "Found lam_ctbignum: building interoperability example")
  # Force C++23 on the imported target to satisfy module requirements
  set_property(TARGET lam::ctbignum APPEND PROPERTY INTERFACE_COMPILE_FEATURES cxx_std_23)

  add_executable(ctbignum_interop examples/ctbignum_interop.cpp)
  target_compile_features(ctbignum_interop PRIVATE cxx_std_23)
  target_link_libraries(ctbignum_interop PRIVATE lam::bitvector lam::ctbignum)
endif()

# Build general examples
add_executable(atkin examples/atkin.cpp)
target_compile_features(atkin PRIVATE cxx_std_23)
target_link_libraries(atkin PRIVATE lam::bitvector)
