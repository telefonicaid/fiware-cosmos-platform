
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
 */

#ifndef _H_SAMSON_wiki_parse_words
#define _H_SAMSON_wiki_parse_words

#include "logMsg/logMsg.h"
#include <samson/module/samson.h>
#include <samson/modules/system/String.h>


namespace samson{
namespace wiki{


class parse_words : public samson::Parser
{
	samson::system::String word;
	samson::system::String page;

public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
	// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

	output: system.String system.String

	helpLine: Parse a web site (in wikipedia format) and emit one kv per word, with the word as key, and the page as value (to build an inverted index)
#endif // de INFO_COMMENT

	void init( samson::KVWriter *writer )
	{
		//OLM_T(LMT_User06, ("parse_words: init"));
	}

	char *strnstr(char *text, const char *pattern, size_t max_length)
	{
		char cpat, ctxt;
		size_t len;

		if ((cpat = *pattern++) != '\0')
		{
			len = strlen(pattern);
			do
			{
				do
				{
					if ((max_length-- < 1) || (ctxt = *text++) == '\0')
						return (NULL);
				} while (ctxt != cpat);
				if (len > max_length)
					return (NULL);
			} while (strncmp(text, pattern, len) != 0);
			text--;
		}
		return (text);
	}

	char *strnmultchr(char *text, const char *stoppers, size_t max_length)
	{
		char *text_end = text + max_length;
		char *p_char = text;

		while (p_char < text_end)
		{
			if (strchr(stoppers, *p_char) != NULL)
			{
				return (p_char);
			}
			p_char++;
		}
		return (NULL);
	}
	char *strnmultchr(char *text, const char *stoppers, char *text_end)
	{
		char *p_char = text;

		while (p_char < text_end)
		{
			if (strchr(stoppers, *p_char) != NULL)
			{
				return (p_char);
			}
			p_char++;
		}
		return (NULL);
	}

	void parseWikiPages(char *text, char *text_end, samson::KVWriter *writer)
	{
		char *p_tag_begin = text;
		char *p_text_end = text_end;
		char *p_end = text_end;

#define PAGE_TAG_TITLE_BEGIN "<title>"
#define PAGE_TAG_TITLE_END "</title>"

		if ((p_tag_begin = strnstr(p_tag_begin, PAGE_TAG_TITLE_BEGIN, text_end - p_tag_begin)) == NULL)
		{
			OLM_E(("page without title begin:'%s'", PAGE_TAG_TITLE_BEGIN));
			return;
		}
		p_tag_begin += strlen(PAGE_TAG_TITLE_BEGIN);
		if ((p_end = strnstr(p_tag_begin, PAGE_TAG_TITLE_END, text_end - p_tag_begin)) == NULL)
		{
			OLM_E(("page without title end:'%s'", PAGE_TAG_TITLE_END));
			return;
		}

		page.value = std::string(p_tag_begin, p_end - p_tag_begin);
		p_tag_begin = p_end + strlen(PAGE_TAG_TITLE_END);

#define TEXT_BEGIN "<text xml:space=\"preserve\">"
#define TEXT_END "</text>"

		while ((p_tag_begin = strnstr(p_tag_begin, TEXT_BEGIN, text_end - p_tag_begin)) != NULL)
		{
			p_tag_begin += strlen(TEXT_BEGIN);
			if ((p_text_end = strnstr(p_tag_begin, TEXT_END, text_end - p_tag_begin)) == NULL)
			{
				OLM_E(("page with text begin at:'%s', but not end", p_tag_begin));
				return;
			}
#define REDIRECT_TAG "#REDIRECT"
			if (strncmp(p_tag_begin, REDIRECT_TAG, strlen(REDIRECT_TAG)) == 0)
			{
				p_tag_begin = p_text_end;
				continue;
			}

#define BLANK_CHARACTERS " \t\n\r[]"
			while ((p_tag_begin < p_text_end) && ((p_end = strnmultchr(p_tag_begin, BLANK_CHARACTERS, p_text_end)) != NULL))
			{

#define LINK_BEGIN "[["
#define LINK_END "]]"
#define LINK_SEP "|"

				char *p_link_end;
				char *p_link_sep;
				if (strncmp(p_tag_begin, LINK_BEGIN, strlen(LINK_BEGIN)) == 0)
				{
					p_tag_begin += strlen(LINK_BEGIN);
					if ((p_link_end = strnstr(p_tag_begin, LINK_END, p_text_end-p_tag_begin)) != NULL)
					{
						if ((p_link_sep = strnstr(p_tag_begin, LINK_SEP, p_link_end-p_tag_begin)) != NULL)
						{
							p_tag_begin = p_link_sep+1;
							if ((p_end = strnmultchr(p_tag_begin, BLANK_CHARACTERS, p_text_end)) == NULL)
							{
								p_end = p_link_end;
							}
						}
					}
				}

				if (((p_end - p_tag_begin) > 100) || (p_end < p_tag_begin))
				{
//					char *copia;
//					copia = strndup(p_tag_begin, 50);
//					OLM_T(LMT_User06, ("p_tag_begin:%lu, p_end-p_tag_begin:%d, p_end:%lu, p_text_end:%lu", p_tag_begin-p_ini_text, p_end-p_tag_begin, p_text_end-p_ini_text, p_end-p_ini_text));
//					OLM_T(LMT_User06, ("word too long:%s", copia));
//					free(copia);
				}
				else
				{
					char *p_char = p_tag_begin;
					while ((p_char < p_end) && !isalpha(*p_char) && !isdigit(*p_char))
					{
						p_char++;
					}
					p_tag_begin = p_char;
					p_char = p_end-1;
					while ((p_char > p_tag_begin) && !isalpha(*p_char) && !isdigit(*p_char))
					{
						p_char--;
					}
					p_end = p_char+1;
					if (p_end > p_tag_begin)
					{
#define FUNCT_CHARACTERS "&|;:/=#[]{}()"
						if ((p_char = strnmultchr(p_tag_begin, FUNCT_CHARACTERS, p_end)) != NULL)
						{
							p_tag_begin++;
							continue;
						}
						word.value = std::string(p_tag_begin, p_end - p_tag_begin);
						//OLM_T(LMT_User06, ("word:%s\n", word.value.c_str()));
						//LM_M(("word:%s\n", word.value.c_str()));
						writer->emit(0, &word, &page);
					}
				}
				p_tag_begin = p_end + 1;
			}
			if (p_tag_begin != p_text_end)
			{
				p_end = p_text_end;
				if (((p_end - p_tag_begin) > 100) || (p_end < p_tag_begin))
				{
//					char *copia;
//					copia = strndup(p_tag_begin, 50);
//					OLM_T(LMT_User06, ("p_tag_begin:%lu, p_end-p_tag_begin:%d, p_end:%lu, p_text_end:%lu", p_tag_begin-p_ini_text, p_end-p_tag_begin, p_text_end-p_ini_text, p_end-p_ini_text));
//					OLM_T(LMT_User06, ("word too long:%s", copia));
//					free(copia);
				}
				else
				{
					char *p_char = p_tag_begin;
					while ((p_char < p_end) && !isalpha(*p_char) && !isdigit(*p_char))
					{
						p_char++;
					}
					p_tag_begin = p_char;
					p_char = p_end-1;
					while ((p_char > p_tag_begin) && !isalpha(*p_char) && !isdigit(*p_char))
					{
						p_char--;
					}
					p_end = p_char+1;
					if (p_end > p_tag_begin)
					{
						if ((p_char = strnmultchr(p_tag_begin, FUNCT_CHARACTERS, p_end-p_tag_begin)) != NULL)
						{
							p_tag_begin++;
							continue;
						}
						word.value = std::string(p_tag_begin, p_text_end - p_tag_begin);
						//OLM_T(LMT_User06, ("word:%s\n", word.value.c_str()));
						//LM_M(("word:%s\n", word.value.c_str()));
						writer->emit(0, &word, &page);
					}
				}
			}
		}

		return;

	}


	void run(char *data, size_t length, samson::KVWriter *writer)
	{
		size_t offset = 0;
		char *p_end_data = data + length;

		char *p_page_begin = data;
		char *p_page_end;

#define PAGE_BEGIN "<page>"
#define PAGE_END "</page>"

		while (offset < length)
		{
			if ((p_page_begin = strnstr(p_page_begin, PAGE_BEGIN, p_end_data-p_page_begin)) == NULL)
			{
				OLM_E(("Error, bad formed page (begin) starting at pos offset:%lu de length:%lu, (%s)", offset, length, p_page_begin));
				return;
			}
			if ((p_page_end = strnstr(p_page_begin, PAGE_END, p_end_data-p_page_begin)) == NULL)
			{
				OLM_E(("Error, bad formed page (end) starting at pos offset:%lu de length:%lu, (%s)", offset, length, p_page_begin));
				return;
			}

			p_page_end += strlen(PAGE_END);

			parseWikiPages(p_page_begin, p_page_end, writer);

			p_page_begin = p_page_end + 1;
			offset = p_page_begin - data;
		}
	}


	void finish( samson::KVWriter *writer )
	{
		//OLM_T(LMT_User06, ("parse_words: finish"));
	}



};


} // end of namespace wiki
} // end of namespace samson

#endif