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

#mpic++ -g -O0 -std=c++11 -Wall -I../simple_serializer -c ../simple_serializer/Serializer.cpp
mpic++ -g -O0 -std=c++11 -Wall -I../simple_serializer -c test_send_serializer_buffer.cpp
mpic++ -g -O0 -std=c++11 -Wall test_send_serializer_buffer.o -o test


