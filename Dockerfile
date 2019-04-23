FROM ubuntu:18.04

RUN apt update -y
RUN apt install gcc-8 -y
RUN apt install make -y
