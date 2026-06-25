cmake_minimum_required(VERSION 3.10)

PROJECT(Metaheuristicas)

file(GLOB C_SOURCES "src/*.cpp" "src/*.c")
include_directories("common" "inc")

# Quitamos explícitamente los mains del GLOB para evitar duplicidades
list(REMOVE_ITEM C_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/maincec.cpp")

add_executable(main "main.cpp" ${C_SOURCES})
add_executable(maincec "maincec.cpp" ${C_SOURCES})


# Enable optimization flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

# include(FetchContent)
# FetchContent_Declare(
#   googletest
#   URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
# )
# set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# FetchContent_MakeAvailable(googletest)

# enable_testing()

# file(GLOB TEST "test/*.cpp")
# add_executable(all_test ${TEST})
# target_link_libraries(all_test GTest::gtest_main)

# gtest_discover_tests(all_test)
