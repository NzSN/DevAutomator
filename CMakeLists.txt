cmake_minimum_required(VERSION 3.5)

# Project define
project(DevAutomator)
set(ROOT .)
set(SRCs ${ROOT}/src)
set(Libs ${ROOT}/lib)
set(CMAKE_C_STANDARD 11)
set(SRCFs ${SRCs}/lib.c)
add_executable(binary ${SRCs}/main.c ${SRCFs})
add_executable(unittest ${SRCFs})
