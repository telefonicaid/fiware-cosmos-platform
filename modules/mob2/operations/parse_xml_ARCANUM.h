
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
 */

#ifndef _H_SAMSON_mob2_parse_xml_ARCANUM
#define _H_SAMSON_mob2_parse_xml_ARCANUM


#include <samson/module/samson.h>
#include <samson/modules/cdr/mobCdr.h>
#include <samson/modules/system/UInt.h>


namespace samson{
namespace mob2{


class parse_xml_ARCANUM : public samson::Parser
{
	//Output
	samson::system::UInt node;
	samson::cdr::mobCdr  cdr;
	samson::system::UInt imei;
	samson::system::UInt32 tac;

public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
	// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

	output: system.UInt cdr.mobCdr
	output: ststem.UInt system.UInt
	output: ststem.UInt system.UInt
	output: ststem.UInt32 system.UInt

	extendedHelp: 		Parse xmls fro Passive Location, generated by ARCANUM
	Emits: phone-cdr info
	phone-IMEI kvs
	IMEI-phone kvs
	TAC-phone kvs

#endif // de INFO_COMMENT

	void init( samson::KVWriter *writer )
	{
	}

	///Function to translate a char
	///into a integer.
	///
	///@param line character to traslate.
	inline int GST_char_to_int(char character)
	{
		return (character -48);
	}

	bool xmlStringToTime(char *strTimeStamp, struct tm *time)
	{
		// TimeStamp format in ARCANUM xmls
		// 2011-07-21T15:44:57
		time->tm_year = 100 + 10*GST_char_to_int(strTimeStamp[2]) + GST_char_to_int(strTimeStamp[3]);
		time->tm_mon = 10*GST_char_to_int(strTimeStamp[5]) + GST_char_to_int(strTimeStamp[6]) - 1;
		time->tm_mday = 10*GST_char_to_int(strTimeStamp[8]) + GST_char_to_int(strTimeStamp[9]);

		time->tm_hour = GST_char_to_int(strTimeStamp[11])*10 + GST_char_to_int(strTimeStamp[12]);
		time->tm_min = GST_char_to_int(strTimeStamp[14])*10 + GST_char_to_int(strTimeStamp[15]);
		time->tm_sec = GST_char_to_int(strTimeStamp[17])*10 + GST_char_to_int(strTimeStamp[18]);

		return(true);
	}

	void parseXML (char *text, samson::KVWriter *writer )
	{
		char *endptr;

		char *p_tag_begin;
		char *p_sep;
		char *p_xml = text;

		struct tm timeExpanded;

		//OLM_T(LMT_User06, ("parseXML called"));

#define XML_TAG_USERID "<IMSI>"
		if ((p_tag_begin = strstr(p_xml, XML_TAG_USERID)) == NULL)
		{
			//OLM_E(("xml without userId"));
			return;
		}
		p_tag_begin += strlen(XML_TAG_USERID);
		if ((p_sep = strchr(p_tag_begin, '<')) == NULL)
		{
			OLM_E(("xml without userId end"));
			return;
		}
		*p_sep = '\0';
		node.value = cdr.phone.value = strtoul(p_tag_begin, &endptr, 10 );
		if (*endptr != '\0')
		{
			OLM_E(("xml with wrong userId:'%s'", p_tag_begin));
			return;
		}
		p_xml = p_sep+1;

#define XML_TAG_IMEI "<IMEI>"
		if ((p_tag_begin = strstr(p_xml, XML_TAG_IMEI)) != NULL)
		{
			p_tag_begin += strlen(XML_TAG_IMEI);
			if ((p_sep = strchr(p_tag_begin, '<')) == NULL)
			{
				OLM_E(("xml without userId end"));
				return;
			}
			*p_sep = '\0';
			imei.value = strtoul(p_tag_begin, &endptr, 10 );
			if (*endptr != '\0')
			{
				OLM_E(("xml with wrong userId:'%s'", p_tag_begin));
				return;
			}
			OLM_T(LMT_User06, ("Emit node:%lu, imei:%lu", node.value, imei.value));
			writer->emit(1, &node, &imei);
			writer->emit(2, &imei, &node);
			if (strlen(p_tag_begin) <= 8)
			{
				OLM_E(("Error selecting TAC for IMEI:%s too short", p_tag_begin));
			}
			else
			{
				p_tag_begin[strlen(p_tag_begin)-8] = '\0';
				tac.value = strtoul(p_tag_begin, &endptr, 10 );
				writer->emit(3, &tac, &node);
			}
			p_xml = p_sep+1;
		}

#define XML_TAG_CELLID "<CellID>"
		if ((p_tag_begin = strstr(p_xml, XML_TAG_CELLID)) == NULL)
		{
			//OLM_E(("xml without cellId"));
			return;
		}
		p_tag_begin += strlen(XML_TAG_CELLID);
		if ((p_sep = strchr(p_tag_begin, '<')) == NULL)
		{
			OLM_E(("xml without cellId end"));
			return;
		}
		*p_sep = '\0';
		cdr.cellId.value = strtoul(p_tag_begin, &endptr, 10 );
		if (*endptr != '\0')
		{
			OLM_E(("xml with wrong cellId:'%s'", p_tag_begin));
			return;
		}
		p_xml = p_sep+1;

#define XML_TAG_TIMESTAMP "<Timestamp>"
		if ((p_tag_begin = strstr(p_xml, XML_TAG_TIMESTAMP)) == NULL)
		{
			OLM_E(("xml without timeStamp"));
			return;
		}
		p_tag_begin += strlen(XML_TAG_TIMESTAMP);
		if ((p_sep = strchr(p_tag_begin, '<')) == NULL)
		{
			OLM_E(("xml without timeStamp end"));
			return;
		}
		*p_sep = '\0';
		if (xmlStringToTime(p_tag_begin, &timeExpanded) == false)
		{
			OLM_E(("xml with wrong timeStamp:'%s'", p_tag_begin));
			return;
		}
		cdr.timeUnix.getTimeUTCFromCalendar(&timeExpanded);

		writer->emit(0, &node, &cdr);
		return;

	}
	void run( char *data , size_t length , samson::KVWriter *writer )
	{
		size_t offset = 0;

		char *p_xml_begin = data;
		char *p_xml_end;

		char *p_xml_doc;

#define XML_BEGIN "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
#define XML_END "</ns0:AMRReport>"

		while( offset < length )
		{
			if (strncmp(p_xml_begin, XML_BEGIN, strlen(XML_BEGIN)))
			{
				OLM_E(("Error, bad formed XML (begin) starting at pos offset:%lu, (%s)", offset, p_xml_begin));
				return;
			}
			if ((p_xml_end = strstr(p_xml_begin, XML_END)) == NULL)
			{
				OLM_E(("Error, bad formed XML (end) starting at pos offset:%d, (%s)", offset, p_xml_begin));
				return;
			}

			p_xml_doc = strndup(p_xml_begin, (p_xml_end - p_xml_begin));
			p_xml_end[strlen(XML_END)] = '\0';

			parseXML (p_xml_begin, writer);

			p_xml_begin = p_xml_end + strlen(XML_END) + 1;
			offset = p_xml_begin - data;
		}
	}

	void finish( samson::KVWriter *writer )
	{
	}



};


} // end of namespace mob2
} // end of namespace samson

#endif