#!/bin/sh

compiler=gcc
compiler_options='-O4 -g0'
outputfile=hellorglory
source=src/main.cpp

projectpath=`awk 'BEGIN{match(ARGV[1],"(.*/)*")}' "$0"`
cd ${projectpath:=`pwd`/}

$compiler $compiler_options -o$outputfile $source
