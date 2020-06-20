#!/bin/bash

if [[ $1 == "debug" ]]; then
    if [[ ! -f "./bin/clue" ]]; then
        make
    fi

    ./bin/clue debug
fi

