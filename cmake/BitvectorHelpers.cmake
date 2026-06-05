# Helper functions for the bitvector module's dev targets (tests, benchmarks,
# examples).
#
# Each of those subdirs builds many small executables that consume the
# lam.bitvector C++23 module in the same way. bitvector_add_executable()
# centralizes that boilerplate (link lam::bitvector, C++23, the optional
# constexpr-step bump some sieve/compile benchmarks need) so it lives in one
# place instead of being re-typed per target.
#
# Modeled on polynomial_nttp's add_polynomial_executable, not ctbignum's helper.

include_guard(GLOBAL)

# bitvector_add_executable(<name>
#   [SOURCE          <file>]    # source file; defaults to <name>.cpp
#   [LIBS            <libs...>] # extra link libraries
#   [DEFINES         <defs...>] # extra PRIVATE compile definitions
#   [CONSTEXPR_LIMIT <n>])      # bump the constexpr step/ops budget (heavy CT work)
#
# Always links lam::bitvector and requests C++23. The global CMAKE_CXX_MODULE_STD
# / CMAKE_CXX_SCAN_FOR_MODULES settings (set in the root CMakeLists) supply the
# module machinery; consuming lam::bitvector carries its usage requirements.
function(bitvector_add_executable NAME)
  cmake_parse_arguments(ARG "" "SOURCE;CONSTEXPR_LIMIT" "LIBS;DEFINES" ${ARGN})

  if(NOT ARG_SOURCE)
    set(ARG_SOURCE "${NAME}.cpp")
  endif()

  add_executable(${NAME} ${ARG_SOURCE})
  target_link_libraries(${NAME} PRIVATE lam::bitvector ${ARG_LIBS})
  target_compile_features(${NAME} PRIVATE cxx_std_23)

  if(ARG_DEFINES)
    target_compile_definitions(${NAME} PRIVATE ${ARG_DEFINES})
  endif()
  if(ARG_CONSTEXPR_LIMIT)
    lam_target_constexpr_limit(${NAME} ${ARG_CONSTEXPR_LIMIT})
  endif()
endfunction()
