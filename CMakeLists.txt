cmake_minimum_required(VERSION 3.5)

# Project define
project(DevAutomator)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
set(ROOT .)
set(SRCs ${ROOT}/src)
set(Libs ${ROOT}/lib)
set(CMAKE_C_STANDARD 11)
set(SRCFs ${SRCs}/list.c ${SRCs}/dict.c)
set(INCLUDE ${ROOT}/include ${ROOT}/include/lib)
add_executable(binary ${SRCs}/main.c ${SRCFs})

set(TESTs ${ROOT}/test)
set(TEST_OBJs
  # List's testcases
  ${TESTs}/list_test.c
  # Dict's testcases
  ${TESTs}/dict_test.c)
add_executable(unittest ${ROOT}/test/test.c ${TEST_OBJs} ${SRCFs})
include_directories(${INCLUDE})
