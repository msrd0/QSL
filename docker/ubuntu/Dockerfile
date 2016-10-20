FROM ubuntu:latest
RUN apt-get -y update && apt-get install -y clang cmake doxygen g++ gcc git graphviz qt5-default
ADD qsl @qsl@
CMD /qsl/docker/build.sh
