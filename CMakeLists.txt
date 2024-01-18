cmake_minimum_required(VERSION 3.0)

project(CRTP_Exam)

add_executable(main src/main.c src/ActorThread.c src/ConsumerThread.c src/ProducerThread.c)

add_executable(clients src/Client.c)