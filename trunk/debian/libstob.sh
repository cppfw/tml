#!/bin/sh

soName=1

packageName=libting$soName

baseDir=debian/out/$packageName
mkdir -p $baseDir

#copy files
libDir=$baseDir/usr/lib
mkdir -p $libDir

libName=libting

libFileName=$libName.so

cp src/$libFileName.$soName $libDir
strip -g $libDir/$libFileName.$soName



#create dir where the output 'control' will be placed
mkdir -p $baseDir/DEBIAN

#remove substvars
rm -f debian/substvars

#create DEBIAN/shlibs file
echo "$libName $soName $packageName (>= `dpkg-parsechangelog | awk '$1 ~ /Version/ {print $2}'`)" > $baseDir/DEBIAN/shlibs

#calculate dependancies
dpkg-shlibdeps $libDir/$libFileName.$soName

#generate final control file
dpkg-gencontrol -p$packageName -P$baseDir

dpkg -b $baseDir tmp-package.deb
dpkg-name -o -s .. tmp-package.deb #rename file to proper debian format (package_version_arch.deb)
