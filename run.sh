#!/bin/bash

(cd build; make)
(cd data; ../build/src/gill-cli walt.yaml > ../build/walt.ppm)
open build/walt.ppm
(cd data; ../build/src/gill-cli bunny.yaml > ../build/bunny.ppm)
open build/bunny.ppm
(cd data; ../build/src/gill-cli woola.yaml > ../build/woola.ppm)
open build/woola.ppm
