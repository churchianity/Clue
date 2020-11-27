
search_dir="."

for entry in "$search_dir"/*.clue
do
    clue "$entry"
    code=$?

    [ $code -eq 0 ] && echo "success | $entry" || echo "failure | $entry, exit code: $code"
done

