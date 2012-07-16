for path in $(find BUILD_COVERAGE -name *.gcno )
do
	target_path=CMakeFiles/$( echo $path | awk 'BEGIN {FS="CMakeFiles"}{print $2}' )
	mkdir -p $(dirname $target_path)
	cp $path $target_path
done
