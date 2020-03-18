cmake_minimum_required(VERSION 3.10)

set(CMAKE_BUILD_TYPE "Debug")  
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")  
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")  

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../bin) 

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBASS libass)

include_directories(${LIBASS_INCLUDE_DIRS})

add_executable(ass2lrc ./ass2lrc.c ./lrc.c)
set_property(TARGET ass2lrc PROPERTY C_STANDARD 11)

target_link_libraries(ass2lrc ${LIBASS_LIBRARIES})