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

mpic++ -g -O0 -std=c++11 -Wall -I../../src/simple_serializer -I../../src/mpi_communicator -c test_mpi_communicator_allgather.cpp
mpic++ -g -O0 -std=c++11 -Wall test_mpi_communicator_allgather.o -o test


