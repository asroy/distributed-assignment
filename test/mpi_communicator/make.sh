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

mpic++ -g -O0 -std=c++11 -Wall -I../../src/simple_serializer -I../../src/mpi_communicator -c test_mpi_communicator.cpp
mpic++ -g -O0 -std=c++11 -Wall test_mpi_communicator.o -o test


