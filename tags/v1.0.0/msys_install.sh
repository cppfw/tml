#!/bin/sh

cp src/libstob.* /usr/lib


#copy header files

incDir=/usr/include/stob
mkdir -p $incDir
cp src/stob/*.hpp $incDir
