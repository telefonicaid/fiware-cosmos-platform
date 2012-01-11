#ifndef UTIL_STRING_H
#define UTIL_STRING_H

#include <string>
#include <algorithm>
using namespace std;


namespace util
{

	
	template<class E, class TR, class A> inline
	std::basic_string<E, TR, A>
	replace_all(std::basic_string<E, TR, A>& str,
				const std::basic_string<E, TR, A>&find_str,
				const std::basic_string<E, TR, A>&replace_str)
	{
		size_t start = 0;
		while ((start = str.find(find_str, start)) != std::basic_string<E, TR, A>::npos)
		{
			str.replace(start, find_str.size(), replace_str);
			start += replace_str.size();
		}

		return str;
	}
	
	template<class E, class TR, class A, class OutIt>
	int
	split_string(const std::basic_string<E, TR, A>& str, E split_char, OutIt out)
	{
		const int len = str.size();
		int start = 0;

		int count = 0;
		while (start <= len)
		{
			int end = start;

			while (end < len && str[end] != split_char)
			{
				end += 1;
			}

			*out = str.substr(start, end - start);
			++out;

			start = end + 1;
			count += 1;
		}

		return count;
	}


	template<class E, class TR, class A, class OutIt>
	int
	split_string(const std::basic_string<E, TR, A>& str,
				 E split_char,
				 E quote_char,
				 OutIt out)
	{
		typedef std::basic_string<E, TR, A> str_type;

		const int len = str.size();
		int start = 0;

		int count = 0;
		while (start <= len)
		{
			if (str[start] != quote_char)	// does not start with a quote
			{
				int end = start;

				while (end < len && str[end] != split_char)
				{
					end += 1;
				}

				*out = str.substr(start, end - start);
				++out;

				start = end + 1;
			}
			else	// starts with a quote
			{
				E specific_quote = str[start];

				start += 1;

				int end = start;
				int quote_count = 0;

				while (end < len && (quote_count % 2 == 0 || str[end] != split_char))
				{
					if (str[end] == quote_char)
					{
						quote_count += 1;
					}
					else
					{
						quote_count = 0;
					}

					end += 1;
				}

				str_type field_str = str.substr(start, end - start - quote_count % 2);

				replace_all(field_str, str_type(2, specific_quote), str_type(1, specific_quote));

				*out = field_str;
				++out;

				start = end + 1;
			}

			count += 1;
		}

		return count;
	}


	template<class E, class TR, class A, class OutIt>
	int
	split_string(const std::basic_string<E, TR, A>& str,
				 const std::basic_string<E, TR, A>& split_set,
				 const std::basic_string<E, TR, A>& quote_set,
				 OutIt out)
	{
		typedef std::basic_string<E, TR, A> str_type;

		const int len = str.size();
		int start = 0;

		int count = 0;
		while (start <= len)
		{
			if (quote_set.find(str[start]) == str_type::npos)	// does not start with a quote
			{
				int end = start;

				while (end < len && split_set.find(str[end]) == str_type::npos)
				{
					end += 1;
				}

				*out = str.substr(start, end - start);
				++out;

				start = end + 1;
			}
			else	// starts with a quote
			{
				E specific_quote = str[start];

				start += 1;

				int end = start;
				int quote_count = 0;

				while (end < len && (quote_count % 2 == 0 || split_set.find(str[end]) == str_type::npos))
				{
					if (str[end] == specific_quote)
					{
						quote_count += 1;
					}
					else
					{
						quote_count = 0;
					}

					end += 1;
				}

				str_type field_str = str.substr(start, end - start - quote_count % 2);

				replace_all(field_str, str_type(2, specific_quote), str_type(1, specific_quote));

				*out = field_str;
				++out;

				start = end + 1;
			}

			count += 1;
		}

		return count;
	}


	template<class E, class InIt>
	std::basic_string< E >
	merge_string(InIt first, InIt last, E delim)
	{
		std::basic_string< E > str;

		if (first < last)
		{
			str += *first;
			++first;

			while (first < last)
			{
				str += delim;
				str += *first;
				++first;
			}
		}

		return str;
	}

	void _ToLower( string& str );
	void _ToLwr( char* str );

	template<class E, class TR, class A> inline
	std::basic_string<E, TR, A>
	trim_left(std::basic_string<E, TR, A>& str, const E trim_char)
	{
		const size_t trim_count = str.find_first_not_of(trim_char);

		if (trim_count == std::basic_string<E, TR, A>::npos)
		{
			str.erase();
		}
		else if (trim_count > 0)
		{
			str.erase(0, trim_count);
		}
		return str;
	}

	template<class E, class TR, class A> inline
	std::basic_string<E, TR, A>
	trim_left(std::basic_string<E, TR, A>& str, const E* trim_set)
	{
		const int trim_count = str.find_first_not_of(trim_set);
		//const std::basic_string<E, TR, A>::size_type trim_count = str.find_first_not_of(trim_set);
		if (trim_count == std::basic_string<E, TR, A>::npos)
		{
			str.erase();
		}
		else if (trim_count > 0)
		{
			str.erase(0, trim_count);
		}
		return str;
	}


	template<class E, class TR, class A> inline
	std::basic_string<E, TR, A>
	trim_right(std::basic_string<E, TR, A>& str, const E trim_char)
	{
		size_t trim_start = str.find_last_not_of(trim_char);
		//std::basic_string<E, TR, A>::size_type trim_start = str.find_last_not_of(trim_char);
		if (trim_start == std::basic_string<E, TR, A>::npos)
		{
			str.erase();
		}
		else if (++trim_start < str.size())
		{
			str.erase(trim_start);
		}
		return str;
	}


	template<class E, class TR, class A> inline
	std::basic_string<E, TR, A>
	trim_right(std::basic_string<E, TR, A>& str, const E* trim_set)
	{
		int trim_start = str.find_last_not_of(trim_set);
		//std::basic_string<E, TR, A>::size_type trim_start = str.find_last_not_of(trim_set);
		if (trim_start == std::basic_string<E, TR, A>::npos)
		{
			str.erase();
		}
		else if (++trim_start < str.size())
		{
			str.erase(trim_start);
		}
		return str;
	}


	template<class E, class TR, class A> inline
	std::basic_string<E, TR, A>
	trim(std::basic_string<E, TR, A>& str, const E trim_char)
	{
		trim_right(str, trim_char);
		trim_left(str, trim_char);
		return str;
	}


	template<class E, class TR, class A> inline
	std::basic_string<E, TR, A>
	trim(std::basic_string<E, TR, A>& str, const E* trim_set)
	{
		trim_right(str, trim_set);
		trim_left(str, trim_set);
		return str;
	}


}  // namespace util


#endif // UTIL_STRING_H

