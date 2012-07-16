
run:
	echo "Running demo to SAMSON system at $(SAMSON_SERVER)"
	delilah -f demo_quijote.txt $(SAMSON_SERVER)

	samsonCat result_lengths_total/ | sort -n > result_lengths_total.txt
	samsonCat result_letter_popular_words | sort > result_letter_popular_words.txt
	samsonCat result_words_total | sort > result_words_total.txt
	make check

check:
	diff result_lengths_total.txt          expected_result_lengths_total.txt
	diff result_words_total.txt            expected_result_words_total.txt
	diff result_letter_popular_words.txt   expected_result_letter_popular_words.txt

reset:
	rm -Rf result*
