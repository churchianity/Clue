
search_dir="."
for entry in "$search_dir"/*.clue
do
    clue "$entry" &>/dev/null
    code=$?

    if [ $code -eq 0 ]; then
        echo -ne "\e[1;32m"
        printf "success"
        echo -ne "\e[0m"
        printf " | %s\n" $entry
    else
        echo -ne "\e[1;31m"
        printf "failure"
        echo -ne "\e[0m"
        printf " | %s\n" $entry
    fi
done

