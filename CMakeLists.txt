cmake_minimum_required(VERSION 2.8.4)
project(floodbot CXX)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror -lpthread")

find_package(Boost REQUIRED COMPONENTS system)

include_directories(${PROJECT_SOURCE_DIR}
	${Boost_INCLUDE_DIR}
)

link_directories(
	${Boost_LIBRARY_DIRS}
)

add_executable(floodbot
	main.cpp
	udp_connection.cpp
	opcode.cpp
	packet.cpp
)

target_link_libraries(floodbot ${Boost_LIBRARIES})
