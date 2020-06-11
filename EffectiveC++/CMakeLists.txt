cmake_minimum_required(VERSION 3.10)
project(EffectiveC__)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp NombreContraint.cpp NombreContraint.h Corps.cpp Corps.h DesignPatternComposite.cpp EfficientException.cpp)
add_executable(EffectiveC__ ${SOURCE_FILES})
#add_executable(EffectivePhilippe mainP.cpp)

add_executable(testCpp mainTest.cpp)