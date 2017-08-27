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

mpic++ -g -O0 -std=c++11 -Wall -I../../src/data_utility -I../data_type -c test_send_serializer_buffer.cpp
mpic++ -g -O0 -std=c++11 -Wall test_send_serializer_buffer.o -o test


