remove_all
add -txt data.txt
upload data_terasort_10GB.txt data.txt
terasort.parse_file data.txt data_bin.txt -create
terasort.reduce_and_export data_bin.txt samson_data.txt -create
download samson_data.txt sorted_terasort_samson_10GB.txt
