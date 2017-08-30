#!/bin/bash
printf "\n"
printf "\n"
printf "\n"
printf "\n"
printf "\n"
printf "\n"
printf "\n"
printf "\n"
printf "\n"


#mpic++ -g -O0 -std=c++11 -Wall -I../../src/data_utility -I../../src/mpi_communicator -I../../src/distributed_assignment -c test_distributed_assignment.cpp
#mpic++ -g -O0 -std=c++11 -Wall test_distributed_assignment.o -o test1

#mpic++ -g -O0 -std=c++11 -Wall -I../../src/data_utility -I../../src/mpi_communicator -I../../src/distributed_assignment -c test_distributed_assignment_ping_pong.cpp
#mpic++ -g -O0 -std=c++11 -Wall test_distributed_assignment_ping_pong.o -o test2

mpic++ -g -O0 -std=c++11 -Wall -I../../src/data_utility -I../../src/mpi_communicator -I../../src/distributed_assignment -c test_distributed_assignment_even_loading.cpp
mpic++ -g -O0 -std=c++11 -Wall test_distributed_assignment_even_loading.o -o test3
