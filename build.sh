#!/bin/bash

build() {
    if [ -d "build/" ]
    then
        echo "build folder already exists skipping creating build directory.\n"
        cd build
    else
        printf "Creating build directory...\n"
        mkdir build
        cd build
        printf "Done!\n"
    fi

    printf "Creating CMake files...\n"
    cmake ../
    printf "Done!\n"

    printf "Building CMake project...\n"
    make
    printf "Done!\n"
}

clean() {
    if [-d "build/" ]
    then
        echo "Cleaning build folder...\n"
        rm -rf build
        echo "Done!\n"
    else
        echo "Build folder already cleaned.\n"
    fi
}

clean_and_build() {
    clean
    build
    cd ..
}

main() {
   while getopts u:a:f: flag
   do
      case "${flag}" in
         c) clean=1;;
         b) build=1;;
      esac
   done

   if [ clean == 1 ] && [ build == 1 ]
   then
       clean_and_build
       exit 0
   fi
   
   if [ clean == 1 ] && [ build == 0 ]
   then
       clean
       cd ..
       exit 0
   fi

   if [ clean == 0 ] && [ build == 1 ]
   then
       build
       cd ..
       exit 0
   fi

   printf "Error no options were provided exiting.\n"
   exit -1
}

main
