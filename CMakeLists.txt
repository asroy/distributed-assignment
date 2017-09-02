cmake_minimum_required(VERSION 3.0.0)
project (DistributedAssignment)

#compiler
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -O0")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")
else(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  message(FATAL_ERROR "unknown CXX compiler: ${CMAKE_CXX_COMPILER_ID}")
endif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

#Mpi
find_package(MPI REQUIRED)

message( "MPI_CXX_COMPILER" ${MPI_CXX_COMPILER} )
message( "MPI_CXX_COMPILE_FLAGS" ${MPI_CXX_COMPILE_FLAGS} )
message( "MPI_CXX_LINK_FLAGS" ${MPI_CXX_LINK_FLAGS} )
message( "MPI_CXX_INCLUDE_PATH" ${MPI_CXX_INCLUDE_PATH} )
message( "MPI_CXX_LIBRARIES" ${MPI_CXX_LIBRARIES} )

include_directories(${MPI_CXX_INCLUDE_PATH})
link_libraries(${MPI_CXX_LIBRARIES})

set(CMAKE_CXX_COMPILE_FLAGS ${CMAKE_CXX_COMPILE_FLAGS} ${MPI_CXX_COMPILE_FLAGS})
set(CMAKE_CXX_LINK_FLAGS ${CMAKE_CXX_LINK_FLAGS} ${MPI_CXX_LINK_FLAGS})


#code
include_directories(core/data_utility)
include_directories(core/serializer)
include_directories(core/mpi_communicator)
include_directories(core/distributed_assignment)

add_subdirectory(core)
add_subdirectory(test)


