remove_all
add -txt data.txt
upload data_1GB.txt data.txt
sort.map data.txt data_bin.txt -create
sort.reduce data_bin.txt samson_data.txt -create
download samson_data.txt sorted_samson_1GB.txt