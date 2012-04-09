
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this data
 */

#ifndef _H_SAMSON_system_TimeSlot
#define _H_SAMSON_system_TimeSlot




#include <samson/modules/system/TimeSlot_base.h>
#include <samson/modules/system/Date.h>
#include <samson/modules/system/DateComplete.h>
#include <samson/modules/system/Time.h>
#include <samson/modules/system/TimeUnix.h>

#undef DEBUG_FILES
#ifdef DEBUG_FILES
#include <iostream>
#include <fstream>
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES


namespace samson{
namespace system{

/**
 * Values for weekday mask.
 */
#define WDAY_NONE	0
#define WDAY_MON	1
#define WDAY_TUE	2
#define WDAY_WED	4
#define WDAY_THU	8
#define WDAY_FRI	16
#define WDAY_SAT	32
#define WDAY_SUN	64
#define WDAY_ALL	127

/**
 * Array of characters, each of which
 * is the bitmask to apply in order
 * to check if a week day is included
 * in a time slot.
 */
char weekDayMask [] = {
		WDAY_NONE,
		WDAY_SUN,
		WDAY_MON,
		WDAY_TUE,
		WDAY_WED,
		WDAY_THU,
		WDAY_FRI,
		WDAY_SAT,
		WDAY_ALL
};


class TimeSlot : public TimeSlot_base
{



public:

	void set(const char *str)
	{
		//size_t _pos = 0;
		std::string _strTs = "";
		std::string _tmp = "";


		// check lenght of the remaining substring
		// before inserting timeslot strings
		// into the array
		if (strlen(str) != 25)
		{

#undef DEBUG_FILES
#ifdef DEBUG_FILES
			{
				std::string filename = "/tmp/debug_TimeSlot.log";
				std::ofstream fs(filename.c_str(), std::ios::app);
				fs << "Error with timespec: '" << str << "'" << std::endl;
				fs.close();
			}
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES

			return;
		}

#undef DEBUG_FILES
#ifdef DEBUG_FILES
		{
			std::string filename = "/tmp/debug_TimeSlot.log";
			std::ofstream fs(filename.c_str(), std::ios::app);
			fs << "ok timespec: '" << str << "'" << std::endl;
			fs.close();
		}
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES

		// Example of TimeSlot string format for working time: "0011111 09:00:00 14:59:59"

		// build weekdays mask

		wdaysMask.value = WDAY_NONE;
		if( str[0] == '1' ) wdaysMask.value |= WDAY_SUN;
		if( str[1] == '1' ) wdaysMask.value |= WDAY_SAT;
		if( str[2] == '1' ) wdaysMask.value |= WDAY_FRI;
		if( str[3] == '1' ) wdaysMask.value |= WDAY_THU;
		if( str[4] == '1' ) wdaysMask.value |= WDAY_WED;
		if( str[5] == '1' ) wdaysMask.value |= WDAY_TUE;
		if( str[6] == '1' ) wdaysMask.value |= WDAY_MON;

#undef DEBUG_FILES
#ifdef DEBUG_FILES
		{
			std::string filename = "/tmp/debug_TimeSlot.log";
			std::ofstream fs(filename.c_str(), std::ios::app);
			fs << "dayMask: '" << wdaysMask.value << "'" << std::endl;
			fs.close();
		}
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES


#define CHAR_TO_INT(v) (v-48)
		initSec.value = 36000*CHAR_TO_INT(str[8]) + 3600*CHAR_TO_INT(str[9]) + 600*CHAR_TO_INT(str[11]) + 60*CHAR_TO_INT(str[12]) + 10*CHAR_TO_INT(str[14]) + CHAR_TO_INT(str[15]);
		finalSec.value = 36000*CHAR_TO_INT(str[17]) + 3600*CHAR_TO_INT(str[18]) + 600*CHAR_TO_INT(str[20]) + 60*CHAR_TO_INT(str[21]) + 10*CHAR_TO_INT(str[23]) + CHAR_TO_INT(str[24]);

#undef DEBUG_FILES
#ifdef DEBUG_FILES
		{
			std::string filename = "/tmp/debug_TimeSlot.log";
			std::ofstream fs(filename.c_str(), std::ios::app);
			fs << "ok initSec: '" << initSec.value << "' finalsec:" <<  finalSec.value << std::endl;
			fs.close();
		}
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES


	}



	/**
	 * Method that tests if a certain date
	 * and time is included into the time
	 * slot.
	 *
	 * @param date SAMSON::system structure that stores the date.
	 * @param time SAMSON::system structure that stores the time.
	 */
	bool includes( samson::system::Date *date, samson::system::Time *time )
	{
		int _sec = 0;

		// check weekday first
		if( (weekDayMask[date->week_day.value+1] & wdaysMask.value) == 0 )
		{
#undef DEBUG_FILES
#ifdef DEBUG_FILES
			{
				std::string filename = "/tmp/debug_TimeSlot.log";
				std::ofstream fs(filename.c_str(), std::ios::app);
				fs << "check (weekDayMask[date->week_day.value(%d)+1] & wdaysMask.value) == 0 ) returns false: date->week_day:" << int(date->week_day.value) << " weekDayMask[date->week_day.value(%d)+1]:" << std::hex << int(weekDayMask[date->week_day.value+1]) << " en wdaysMask:" << std::hex << int(wdaysMask.value) << std::endl;
				fs.close();
			}
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES

			return false;
		}

#undef DEBUG_FILES
#ifdef DEBUG_FILES
		{
			std::string filename = "/tmp/debug_TimeSlot.log";
			std::ofstream fs(filename.c_str(), std::ios::app);
			fs << "check (weekDayMask[date->week_day.value(%d)+1] & wdaysMask.value) == 0 ) returns true: date->week_day:" << int(date->week_day.value) << " weekDayMask[date->week_day.value(%d)+1]:" << std::hex << int(weekDayMask[date->week_day.value+1]) << " en wdaysMask:" << std::hex << int(wdaysMask.value) << std::endl;
			fs.close();
		}
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES

		// check day time then
		_sec = 3600*(int)(time->hour.value) + 60*(int)(time->minute.value) + (int)time->seconds.value;
		if( (_sec < initSec.value) || (_sec > finalSec.value) )
		{
#undef DEBUG_FILES
#ifdef DEBUG_FILES
			{
				std::string filename = "/tmp/debug_TimeSlot.log";
				std::ofstream fs(filename.c_str(), std::ios::app);
				fs << "check returns false: _sec:" << _sec << " initSec: "<< initSec.value << "' finalsec:" <<  finalSec.value << std::endl;
				fs.close();
			}
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES
			return false;
		}

#undef DEBUG_FILES
#ifdef DEBUG_FILES
		{
			std::string filename = "/tmp/debug_TimeSlot.log";
			std::ofstream fs(filename.c_str(), std::ios::app);
			fs << "check returns true: _sec:" << _sec << " initSec: "<< initSec.value << "' finalsec:" <<  finalSec.value << std::endl;
			fs.close();
		}
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES





		return true;
	}

	/**
	 * Method that tests if a certain date
	 * and time is included into the time
	 * slot.
	 *
	 * @param date SAMSON::system structure that stores the date.
	 * @param time SAMSON::system structure that stores the time.
	 */
	bool includes( samson::system::DateComplete *date, samson::system::Time *time )
	{
		int _sec = 0;

		// check weekday first
		if( (weekDayMask[date->week_day.value+1] & wdaysMask.value) == 0 )
		{
			return false;
		}

		// check day time then
		_sec = 3600*(int)(time->hour.value) + 60*(int)(time->minute.value) + (int)time->seconds.value;
		if( (_sec < initSec.value) || (_sec > finalSec.value) )
		{
			return false;
		}

		return true;
	}


	/**
	 * Method that tests if a certain date
	 * and time is included into the time
	 * slot.
	 *
	 * @param timeUnix SAMSON::system structure that stores the time.
	 */
	bool includes( samson::system::TimeUnix *time )
	{
		int _sec = 0;
		struct tm timeCalendar;

		time->getCalendarFromTimeUTC(&timeCalendar);

		// check weekday first
		if( (weekDayMask[timeCalendar.tm_wday+1] & wdaysMask.value) == 0 )
		{
			return false;
		}

		// check day time then
		_sec = 3600*(int)(timeCalendar.tm_hour) + 60*(int)(timeCalendar.tm_min) + (int)timeCalendar.tm_sec;
		if( (_sec < initSec.value) || (_sec > finalSec.value) )
		{
			return false;
		}



		return true;
	}


};


} // end of namespace samson
} // end of namespace system

#endif