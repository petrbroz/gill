#!/bin/bash

(cd build; make)
(cd data; cat simple_1.yaml | ../build/src/gill-cli > ../build/simple_1.ppm 2> ../build/simple_1.json)
open build/simple_1.ppm
(cd data; cat simple_2.yaml | ../build/src/gill-cli > ../build/simple_2.ppm 2> ../build/simple_2.json)
open build/simple_2.ppm
