# Conan 2.x recipe for lam.bitvector.
#
# Local development:
#   conan create . --profile <your-profile>
#
# Consumer projects can then declare:
#   requires = "lam_bitvector/<version>"   # version comes from the VERSION file
#
# NOTE: bitvector has no LICENSE yet, so this recipe omits the `license` field
# and license packaging. Add both when the project is licensed. CPS metadata
# (install(PACKAGE_INFO)) is likewise deferred — the recipe ships only the
# legacy *Config.cmake the hand-rolled install emits. The recipe delegates
# entirely to CMake.

from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout
from conan.tools.files import load
import os

class LamBitvectorConan(ConanFile):
    name = "lam_bitvector"
    author = "Colin Ford"
    url = "https://github.com/colinrford/bitvector"
    description = (
        "A C++23-module bit vector (dynamic bitset) for the lam project."
    )
    topics = ("c++23", "modules", "bitvector", "bitset", "lam")

    settings = "os", "compiler", "build_type", "arch"
    package_type = "static-library"

    def set_version(self):
        # Single source of truth: the top-level VERSION file, which CMakeLists
        # also reads. Editing VERSION updates both the CMake project version and
        # this recipe — they never drift.
        self.version = load(
            self, os.path.join(self.recipe_folder, "VERSION")
        ).strip()

    # No external Conan deps: the bitvector library is standalone (only the
    # ctbignum_interop example imports lam.ctbignum, and examples aren't packaged).

    exports_sources = (
        "VERSION",
        "CMakeLists.txt",
        "bitvector_config.cppm.in",
        "src/*",
        "cmake/*",
        "README.md",
    )

    def layout(self):
        cmake_layout(self)

    def validate(self):
        cppstd = self.settings.compiler.cppstd
        if cppstd is not None:
            std = int(str(cppstd).replace("gnu", ""))
            if std < 23:
                raise Exception(
                    "lam_bitvector requires C++23 (compiler.cppstd >= 23)."
                )

    def generate(self):
        tc = CMakeToolchain(self)
        # Mirror what the project's CMakeLists already assumes.
        tc.cache_variables["CMAKE_CXX_STANDARD"] = "23"
        tc.cache_variables["CMAKE_CXX_SCAN_FOR_MODULES"] = "ON"
        # tests/ benchmarks/ examples/ aren't in exports_sources.
        tc.cache_variables["LAM_BITVECTOR_BUILD_TESTS"] = "OFF"
        tc.cache_variables["LAM_BITVECTOR_BUILD_BENCHMARKS"] = "OFF"
        tc.cache_variables["LAM_BITVECTOR_BUILD_EXAMPLES"] = "OFF"
        tc.generate()
        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        # Match the CMake export names so find_package(lam_bitvector) and the
        # Conan-generated CMakeDeps both yield the same target.
        self.cpp_info.set_property("cmake_file_name", "lam_bitvector")
        self.cpp_info.set_property("cmake_target_name", "lam_bitvector::bitvector")
        self.cpp_info.libs = ["lam_bitvector"]
        self.cpp_info.builddirs = [
            os.path.join("lib", "cmake", "lam_bitvector"),
        ]
