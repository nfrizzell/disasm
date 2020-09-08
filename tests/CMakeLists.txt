cmake_minimum_required(VERSION 3.10)
project(disasm-tests VERSION 1.0)

set(SOURCE_FILES ../src/exec.cpp ../src/util/common.cpp ../src/util/safebool.cpp ../src/format/format.cpp ../src/format/elf.cpp ../src/util/util.cpp ../src/arch/arch.cpp ../src/arch/x86/decode.cpp ../src/arch/x86/x86.cpp ../src/arch/x86/enum.cpp ../src/arch/x86/translate.cpp ../src/arch/x86/instruction.cpp)

set(TEST_SOURCE_FILES ./main.cpp ./x86-test.cpp ./elf-test.cpp) 
add_executable(disasm-tests ${TEST_SOURCE_FILES} ${SOURCE_FILES})

target_include_directories(disasm-tests PRIVATE ../include)
set_property(TARGET disasm-tests PROPERTY CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Debug)
set_target_properties(disasm-tests PROPERTIES COMPILE_OPTIONS "-m32;-Wall" LINK_FLAGS "-m32")
