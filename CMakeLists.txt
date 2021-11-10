cmake_minimum_required(VERSION 3.16)

enable_testing()

# Project define
project(DevAutomator)
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -g")
set(ROOT .)
set(SRCs ${ROOT}/src)
set(Libs ${ROOT}/lib)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS)
set(SRCFs
  ${SRCs}/tcdb/tcdb_drivers.cc
  ${SRCs}/tcdb/tcdb.cc
  ${SRCs}/tcdb/tcdb_connector.cc
  ${SRCs}/interpreter.cc
  ${SRCs}/utility/pyFuncEvaluate.cc
  ${SRCs}/interpreter.cc
  ${SRCs}/instruction/instruction.cc
  )

set(INCLUDE
  ${ROOT}/include
  ${ROOT}/include/lib
  ${ROOT}/src ${ROOT}/src/tcdb)

# Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


# Add Python Support
set(Python_ADDITIONAL_VERSIONS 3.8)
find_package(PythonLibs 3 REQUIRED)


add_executable(binary ${SRCs}/main.cc ${SRCFs})
target_link_libraries(binary ${PYTHON_LIBRARIES})
include_directories(${PYTHON_INCLUDE_DIRS})


set(TESTs ${ROOT}/test)
set(TEST_OBJs
  # TCDB Driver testcases
  ${TESTs}/tcdbDriver_test.cc
  # TCDB testcases
  ${TESTs}/tcdb_test.cc
  # TCDB Connector testcases
  ${TESTs}/tcdbConnector_test.cc
  # Interpreter testcases
  ${TESTs}/interpreter_test.cc
  # Utility testcases
  ${TESTs}/utility_test.cc)


add_executable(unittest ${TEST_OBJs} ${SRCFs})
include_directories(${INCLUDE})

# Link Python
target_link_libraries(unittest ${PYTHON_LIBRARIES})
include_directories(${PYTHON_INCLUDE_DIRS})

# Link Google Test
target_link_libraries(unittest gtest_main)

# Memory check
add_test(NAME UNITTESTING_WITH_MEMORY_LEAK_CHECK
  	COMMAND valgrind --leak-check=full
                         --show-leak-kinds=all
                         --track-origins=yes
			 --error-exitcode=1
                         --verbose ./unittest)
