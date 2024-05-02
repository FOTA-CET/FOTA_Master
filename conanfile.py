import os
import re
from conan import ConanFile, tools
from conan.tools.cmake import cmake_layout, CMake, CMakeDeps, CMakeToolchain

class testApp(ConanFile):
    name = "TestApp"
    version = "1.0.0"
    license = "<Put the package license here>"
    author = "<Nguyen Phi Hung> <01hungnph01@gmail.com>"
    url = "<Package recipe repository url here, for issues about the package>"
    description = "<Description of hello package here>"
    topics = ("<Put some tag here>", "<here>", "<and here>")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": False}

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = ["CMakeLists.txt", "src/*", "test/*"]
    
    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC
    
    def layout(self):
        cmake_layout(self)
    
    def requirements(self):
        self.requires("libcurl/8.6.0")
    
    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
    
    def imports(self):
        self.copy("*.so*", keep_path=True)
        self.copy("*.h", keep_path=True)
        self.copy("*.hh", keep_path=True)
        self.copy("*.hpp", keep_path=True)

    def package(self):
        self.copy("fotaClient.hh", dst="include", src="src", keep_path=True)
        self.copy("*.so", dst="lib", keep_path=True)

    def package_info(self):
        pass
    
    