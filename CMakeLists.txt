cmake_minimum_required(VERSION 3.5)

enable_testing()

# Project define
project(DevAutomator)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
set(ROOT .)
set(SRCs ${ROOT}/src)
set(Libs ${ROOT}/lib)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS)
set(SRCFs ${SRCs}/tcdb/tcdb_drivers.cc)
set(INCLUDE ${ROOT}/include ${ROOT}/include/lib 
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

add_executable(binary ${SRCs}/main.cc ${SRCFs})

set(TESTs ${ROOT}/test)
set(TEST_OBJs
  # TCDB Driver testcases
  ${TESTs}/tcdbDriver_test.cc)
add_executable(unittest ${TEST_OBJs} ${SRCFs})
target_link_libraries(unittest gtest_main)
add_test(NAME valgrind_unittest
  	COMMAND valgrind --leak-check=full
                         --show-leak-kinds=all
                         --track-origins=yes
			 --error-exitcode=1
                         --verbose ./unittest)
include_directories(${INCLUDE})
