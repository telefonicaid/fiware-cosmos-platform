before="$(date +%s)"
sort -n --buffer-size=2000M  data.txt > sort_data.txt
after="$(date +%s)"
elapsed_seconds="$(expr $after - $before)"
echo Elapsed time for code block: $elapsed_seconds