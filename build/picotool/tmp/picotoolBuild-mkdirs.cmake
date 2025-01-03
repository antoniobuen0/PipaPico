# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/anto/Proyectos/PipaPico/build/_deps/picotool-src"
  "/Users/anto/Proyectos/PipaPico/build/_deps/picotool-build"
  "/Users/anto/Proyectos/PipaPico/build/_deps"
  "/Users/anto/Proyectos/PipaPico/build/picotool/tmp"
  "/Users/anto/Proyectos/PipaPico/build/picotool/src/picotoolBuild-stamp"
  "/Users/anto/Proyectos/PipaPico/build/picotool/src"
  "/Users/anto/Proyectos/PipaPico/build/picotool/src/picotoolBuild-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/anto/Proyectos/PipaPico/build/picotool/src/picotoolBuild-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/anto/Proyectos/PipaPico/build/picotool/src/picotoolBuild-stamp${cfgdir}") # cfgdir has leading slash
endif()
