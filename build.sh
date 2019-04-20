
# docker build -t ubuntu:gcc-8 . 
docker run -v $(readlink -f .):/src -it ubuntu:gcc-8
