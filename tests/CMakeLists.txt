# Unit tests. bitvector_add_executable (cmake/BitvectorHelpers.cmake) handles the
# module-consumer boilerplate; this just names each test and registers it.
function(lam_add_unit_test name source)
  bitvector_add_executable(${name} SOURCE ${source})
  add_test(NAME ${name} COMMAND ${name})
endfunction()

# Core Tests
lam_add_unit_test(core_construction_test unit/core_construction_test.cpp)
lam_add_unit_test(core_assignment_test   unit/core_assignment_test.cpp)
lam_add_unit_test(core_allocator_test    unit/core_allocator_test.cpp)
lam_add_unit_test(core_print_test        unit/core_print_test.cpp)
lam_add_unit_test(core_metadata_test     unit/core_metadata_test.cpp)
lam_add_unit_test(core_access_test       unit/core_access_test.cpp)
lam_add_unit_test(core_proxy_test        unit/core_proxy_test.cpp)
lam_add_unit_test(core_modification_test unit/core_modification_test.cpp)
lam_add_unit_test(core_utilities_test    unit/core_utilities_test.cpp)
lam_add_unit_test(core_swap_test         unit/core_swap_test.cpp)

# Boolean Ops Tests
lam_add_unit_test(ops_boolean_test       unit/ops_boolean_test.cpp)

# Secure Allocator Proof-of-Concept
lam_add_unit_test(secure_allocator_test  unit/secure_allocator_test.cpp)
lam_add_unit_test(alias_test             unit/alias_test.cpp)
lam_add_unit_test(sieve_test             unit/sieve_test.cpp)
