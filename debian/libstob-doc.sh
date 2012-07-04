#!/bin/sh

packageName=libting-doc

baseDir=debian/out/$packageName
mkdir -p $baseDir

#copy files
docDir=$baseDir/usr/share/doc/libting
mkdir -p $docDir

cp docs/index.htm $docDir
cp -r docs/html_doc_files $docDir



#create dir where the output 'control' will be placed
mkdir -p $baseDir/DEBIAN

#remove substvars
rm -f debian/substvars

#generate final control file
dpkg-gencontrol -p$packageName -P$baseDir

dpkg -b $baseDir tmp-package.deb
dpkg-name -o -s .. tmp-package.deb #rename package file to proper debian format (package_version_arch.deb)
