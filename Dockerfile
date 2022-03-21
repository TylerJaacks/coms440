FROM gcc:11

RUN apt-get update && apt-get -qq install gdb cmake git

COPY Tests/* Tests/