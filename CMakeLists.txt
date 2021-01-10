cmake_minimum_required(VERSION 3.10)
enable_testing()
project(disasm VERSION 1.0)

set(SOURCE_FILES ./src/main.cpp ./src/exec.cpp ./src/util/common.cpp ./src/format/format.cpp ./src/format/elf.cpp ./src/util/util.cpp ./src/arch/arch.cpp ./src/arch/x86/decode.cpp ./src/arch/x86/x86.cpp ./src/arch/x86/translate.cpp ./src/arch/x86/instruction.cpp ./src/arch/x86/csv.cpp)

add_executable(disasm ${SOURCE_FILES})

target_include_directories(disasm PRIVATE include)
set_property(TARGET disasm PROPERTY CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Debug)
set_target_properties(disasm PROPERTIES COMPILE_OPTIONS "-m32;-O3;-Wall;-Wfatal-errors" LINK_FLAGS "-m32")
