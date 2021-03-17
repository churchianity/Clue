#!/bin/bash

red="\033[0;31m"
green="\033[0;32m"
yellow="\033[0;33m"
reset="\033[0m"

if [[ -f "makefile" ]]; then
    echo -e "\n${yellow}Compiling...${reset}"
    make
else
    echo -e "\n${red}Couldn't find makefile.${reset}\nAre you in the root directory?\n"
    exit
fi

if [ $? -ne 0 ]; then
    echo -e "\n${red}Compilation failed, not running any tests.${reset}\n"
    exit
fi

clue_path="./bin/clue"
search_dir="./test/"
echo -e "\n${yellow}Running tests...${reset}"
for entry in "$search_dir"/*.clue
do
    $clue_path "$entry" &>/dev/null
    code=$?

    if [ $code -eq 127 ]; then
        echo "Can't find the clue executable at ${clue_path}"
	exit

    elif [ $code -eq 0 ]; then
        echo -e "${green}success${reset} | ${entry}"
    else
        echo -e "${red}failure${reset} | ${entry}"
    fi
done

