#!/bin/bash

green="\033[0;32m"
red="\033[0;31m"
reset="\033[0m"

search_dir="."
for entry in "$search_dir"/*.clue
do
    clue "$entry" &>/dev/null
    code=$?

    if [ $code -eq 0 ]; then
        echo -e "${green}success${reset} | ${entry}"
    else
        echo -e "${red}failure${reset} | ${entry}"
    fi
done

