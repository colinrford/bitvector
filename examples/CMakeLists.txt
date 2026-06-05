# Examples. bitvector_add_executable (cmake/BitvectorHelpers.cmake) handles the
# lam.bitvector link + C++23.

bitvector_add_executable(atkin SOURCE atkin.cpp)

# eratosthenes does heavy compile-time sieving; bump the constexpr budget.
bitvector_add_executable(eratosthenes
  SOURCE          eratosthenes.cpp
  CONSTEXPR_LIMIT 900000000)

# ctbignum_interop also imports lam.ctbignum. That's a cross-module dependency,
# so build it only when ctbignum is a sibling target in the same build tree (the
# umbrella lam build) and silently skip otherwise — the lam "build what you can"
# convention. We intentionally do NOT find_package(lam_ctbignum): an installed
# ctbignum is consumed via its module file set, which is the natural home for
# this interop demo only when both modules build together.
if(TARGET lam::ctbignum)
  bitvector_add_executable(ctbignum_interop
    SOURCE ctbignum_interop.cpp
    LIBS   lam::ctbignum)
else()
  message(STATUS "bitvector: lam.ctbignum not a sibling target — skipping the "
                 "ctbignum_interop example (build it within the umbrella lam tree to enable).")
endif()
