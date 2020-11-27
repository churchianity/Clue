
search_dir="."

for entry in "$search_dir"/*.clue
do
    clue -i "$entry"
done

