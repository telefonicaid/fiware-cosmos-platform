/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
 */

#ifndef _H_SAMSON_wiki_parse_pages_to_links
#define _H_SAMSON_wiki_parse_pages_to_links

#include <samson/module/samson.h>
#include <samson/modules/system/String.h>
#include <samson/modules/system/StringVector.h>

namespace samson
{
namespace wiki
{

class parse_pages_to_links: public samson::Parser
{

	samson::system::String key;
	samson::system::StringVector value;

public:

#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
	// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

	output: system.String system.StringVector

	helpLine: Parse a txt files emiting words at the output (value is always one)
#endif // de INFO_COMMENT
	void init(samson::KVWriter *writer)
	{
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

	void parseWikiPages(char *text, char *text_end, samson::KVWriter *writer)
	{
		char *p_tag_begin = text;
		char *p_end;
		char *p_sep_link;

		value.valuesSetLength(0);

#define PAGE_TAG_TITLE_BEGIN "<title>"
#define PAGE_TAG_TITLE_END "</title>"

		if ((p_tag_begin = strnstr(p_tag_begin, PAGE_TAG_TITLE_BEGIN, text_end - p_tag_begin)) == NULL)
		{
			return;
		}
		p_tag_begin += strlen(PAGE_TAG_TITLE_BEGIN);
		if ((p_end = strnstr(p_tag_begin, PAGE_TAG_TITLE_END, text_end - p_tag_begin)) == NULL)
		{
			return;
		}

		key.value = std::string(p_tag_begin, p_end - p_tag_begin);
		p_tag_begin = p_end + strlen(PAGE_TAG_TITLE_END);

#define LINK_BEGIN "[["
#define LINK_END "]]"
#define LINK_SEP "|"
#define IMAGE_LINK "[[Image:"
#define FILE_LINK "[[File:"

		while ((p_tag_begin = strnstr(p_tag_begin, LINK_BEGIN, text_end - p_tag_begin)) != NULL)
		{
			p_tag_begin += strlen(LINK_BEGIN);
			if ((p_end = strnstr(p_tag_begin, LINK_END, text_end - p_tag_begin)) == NULL)
			{
				return;
			}
			char *p_intern;
			if ((p_intern = strnstr(p_tag_begin, LINK_BEGIN, p_end - p_tag_begin)) != NULL)
			{
					p_end = p_intern;
			}
#define END_LINE "\n"
			if ((p_intern = strnstr(p_tag_begin, END_LINE, p_end - p_tag_begin)) != NULL)
			{
				p_end = p_intern;
			}
			if ((p_sep_link = strnstr(p_tag_begin, LINK_SEP, p_end - p_tag_begin)) != NULL)
			{
				p_end = p_sep_link;
			}
			value.valuesAdd()->value = std::string(p_tag_begin, p_end - p_tag_begin);
			p_tag_begin = p_end + strlen(LINK_END);
		}

		writer->emit(0, &key, &value);
		return;

	}

	void run(char *data, size_t length, samson::KVWriter *writer)
	{
		size_t offset = 0;

		char *p_page_begin = data;
		char *p_page_end;

#define PAGE_BEGIN "<page>"
#define PAGE_END "</page>"

		while (offset < length)
		{
			if ((p_page_begin = strstr(p_page_begin, PAGE_BEGIN)) == NULL)
			{
				//O_LOG_E(("Error, bad formed page (begin) starting at pos offset:%lu, (%s)", offset, p_page_begin));
				return;
			}
			if ((p_page_end = strstr(p_page_begin, PAGE_END)) == NULL)
			{
				//O_LOG_E(("Error, bad formed page (end) starting at pos offset:%d, (%s)", offset, p_page_begin));
				return;
			}

			p_page_end += strlen(PAGE_END);

			parseWikiPages(p_page_begin, p_page_end, writer);

			p_page_begin = p_page_end + 1;
			offset = p_page_begin - data;
		}
	}

	void finish(samson::KVWriter *writer)
	{
	}

};

} // end of namespace wiki
} // end of namespace samson

#endif
