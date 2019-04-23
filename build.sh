#!/bin/bash 
mkdir -p bin build
docker build -t ubuntu:gcc-8 . 
docker run --rm -v $(readlink -f .):/src -it ubuntu:gcc-8 bash -c 'cd /src; make clean; make; chmod 777 bin/* build/*'

