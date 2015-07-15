#!/bin/bash

(cd build; make)
(cd data; ../build/src/gill-cli simple_1.yaml > ../build/simple_1.ppm 2> ../build/simple_1.json)
open build/simple_1.ppm
(cd data; ../build/src/gill-cli simple_2.yaml > ../build/simple_2.ppm 2> ../build/simple_2.json)
open build/simple_2.ppm
