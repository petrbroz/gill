#!/bin/bash

process()
{
    (cd data; ../build/src/gill-cli ${1}.yaml > ../build/${1}.ppm 2> ../build/${1}.log)
    logs=`cat build/${1}.log`
    convert build/${1}.ppm -gravity SouthWest -fill White -annotate 0 "$logs" build/${1}.png
    rm build/${1}.ppm build/${1}.log
    open build/${1}.png
}

(cd build; make)
process walt
process bunny
process woola
