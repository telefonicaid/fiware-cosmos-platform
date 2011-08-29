
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this data
 */

#ifndef _H_SAMSON_system_TimeUnix
#define _H_SAMSON_system_TimeUnix

#include <time.h>
#include <limits.h>

#include <samson/modules/system/FixedLengthDataInstance.h>
#include <samson/modules/system/Date.h>
#include <samson/modules/system/DateComplete.h>
#include <samson/modules/system/Time.h>

#undef DEBUG_FILES
#ifdef DEBUG_FILES
#include <iostream>
#include <fstream>
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES


namespace samson{
namespace system{


/**
		 TimeUnix  Data Instance 
 */

class TimeUnix : public FixedLengthDataInstance<time_t>{

	inline int tmcomp(register const struct tm * const atmp,
			register const struct tm * const btmp)
	{
		register int	result;

		if ((result = (atmp->tm_year - btmp->tm_year)) == 0 &&
				(result = (atmp->tm_mon - btmp->tm_mon)) == 0 &&
				(result = (atmp->tm_mday - btmp->tm_mday)) == 0 &&
				(result = (atmp->tm_hour - btmp->tm_hour)) == 0 &&
				(result = (atmp->tm_min - btmp->tm_min)) == 0)
			result = atmp->tm_sec - btmp->tm_sec;
		return result;
	}

public:


	int hash(int max_num_partitions){
		return value%max_num_partitions;
	}


	int *getDataPath(const std::string &dataPathString){
		return(getDataPathStatic(dataPathString));
	}

	static int *getDataPathStatic(const std::string &dataPathString){
		const char *dataPathCharP = dataPathString.c_str();
		int nlevels = 1;
		int *dataPathIntP;

		const char *p_sep = dataPathCharP;
		while ((p_sep = strchr(p_sep, '.')) != NULL)
		{
			nlevels++;
			p_sep++;
		}

		if ((dataPathIntP = (int *)malloc((nlevels + 1)*sizeof(int))) == NULL)
		{
			return ((int *)NULL);
		}

		int retError = getDataPath(dataPathCharP, dataPathIntP);

		if (retError)
		{
			free(dataPathIntP);
			dataPathIntP = NULL;
		}

		return  (dataPathIntP);
	}

	static int getDataPath(const char * dataPathCharP, int *dataPathIntP){
		if (*dataPathCharP == 0)
		{
			*dataPathIntP = -1;
			return (0);
		}

		if (strcmp(dataPathCharP, "TimeUnix") == 0)
		{
			*dataPathIntP = -1;
			return (0);
		}

		return -1;
	}

	std::string getTypeFromPath(const std::string &dataPathString){
		const char *dataPathCharP = dataPathString.c_str();
		return(getTypeFromPathStatic(dataPathCharP));
	}

	static std::string getTypeFromPathStatic(const char * dataPathCharP){
		if (*dataPathCharP == 0)
		{
			return ("system.TimeUnix");
		}

		if (strcmp(dataPathCharP, ".") == 0)
		{
			return ("system.TimeUnix");
		}

		if (strcmp(dataPathCharP, "TimeUnix") == 0)
		{
			return ("system.TimeUnix");
		}
		return("_ERROR_");
	}

	std::string getTypeFromPath(const int *dataPathIntP){
		return(getTypeFromPathStatic(dataPathIntP));
	}

	static std::string getTypeFromPathStatic(const int *dataPathIntP){
		switch(*dataPathIntP)
		{
		case -1:
			return ("system.TimeUnix");
			break;
		default:
			return ("_ERROR_");
			break;
		};
	}

	static const char *getTypeStatic()
	{
		return ("system.TimeUnix");
	}

	const char *getType()
	{
		return ("system.TimeUnix");
	}

	static bool checkTypeStatic(const char *type)
	{
		if (strcmp(type, "system.TimeUnix") == 0)
		{
			return true;
		}
		return false;
	}

	bool checkType(const char *type)
	{
		if (strcmp(type, "system.TimeUnix") == 0)
		{
			return true;
		}
		return false;
	}

	static size_t getHashTypeStatic(){
		return(7071866688332326601ULL);
	}

	size_t getHashType(){
		return(7071866688332326601ULL);
	}

	static bool checkHashTypeStatic(size_t valType){
		if (valType == 7071866688332326601ULL)
		{
			return true;
		}		return false;
	}

	 bool checkHashType(size_t valType){
		if (valType == 7071866688332326601ULL)
		{
			return true;
		}		return false;
	}


	DataInstance * getDataInstanceFromPath(const int *dataPathIntP){
		switch(*dataPathIntP)
		{
		case -1:
			return (this);
			break;
		default:
			return (NULL);
			break;
		};
	}

	std::string str()
	{
		std::ostringstream o;
		char buffAscTime[27];
		struct tm timeExpanded;
		getCalendarFromTimeUTC(&timeExpanded);
		asctime_r(&timeExpanded, buffAscTime);
		buffAscTime[strlen(buffAscTime) - 1] = '\0';
		o << buffAscTime << " ";
		return o.str();
	}

	void getTimeFromCalendar (struct tm *tm)
	{
		value = mktime(tm);
	}

	struct tm *getCalendarFromTime (struct tm *result)
	{
		return (localtime_r(&value, result));
	}




	void getTimeUTCFromCalendar_shiftandcompare(struct tm *tm)
	{

		// Copied from utc_mktime
		// Idea of Bob Kridle (so its said...).
		// http://www.koders.com/c/fidB1A0A680FBCF84DDC534E66371053D726A5546EB.aspx?s=md5
		// Very cool, but not so efficient for 64 bits long time_t

		register int			dir;
		register int			bits;
		register int			saved_seconds;
		time_t				t;
		struct tm			yourtm, mytm;

		yourtm = *tm;
		saved_seconds = yourtm.tm_sec;
		yourtm.tm_sec = 0;
		/*
		 ** Calculate the number of magnitude bits in a time_t
		 ** (this works regardless of whether time_t is
		 ** signed or unsigned, though lint complains if unsigned).
		 **
		 ** We check TIME_T_MAX_BITS beforehand since gmtime() may fail
		 ** with large 64bit values in some systems.
		 */
#define TIME_T_MAX_BITS 64
		for (bits = 0, t = 1; t > 0 && bits < TIME_T_MAX_BITS-1; bits++)
			t <<= 1;

		/*
		 ** If time_t is signed, then 0 is the median value,
		 ** if time_t is unsigned, then 1 << bits is median.
		 */
		t = (t < 0) ? 0 : ((time_t) 1 << bits);
		for ( ; ; ) {
			gmtime_r(&t, &mytm);
			dir = tmcomp(&mytm, &yourtm);
			if (dir != 0) {
				if (bits-- < 0)
				{
					value = (time_t) -1;
					return;
				}
				if (bits < 0)
					--t;
				else if (dir > 0)
					t -= (time_t) 1 << bits;
				else	t += (time_t) 1 << bits;
				continue;
			}
			break;
		}
		t += saved_seconds;
		value = t;
	}


#define  YEAR0   1900
#define  EPOCH_YR   1970
#define  SECS_DAY   (24L * 60L * 60L)
#define  LEAPYEAR(year)   (!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define  YEARSIZE(year)   (LEAPYEAR(year) ? 366 : 365)
#define  FIRSTSUNDAY(timp)   (((timp)->tm_yday - (timp)->tm_wday + 420) % 7)
#define  FIRSTDAYOF(timp)   (((timp)->tm_wday - (timp)->tm_yday + 420) % 7)
#define  TIME_MAX   ULONG_MAX
#define  ABB_LEN   3

	void getTimeUTCFromCalendar(struct tm *tm)
	{
		register long day = 0;
		register long year = EPOCH_YR;
		register int tm_year;
		int yday, month;
		register unsigned long seconds = 0;
		//int overflow = 0;
		//unsigned dst;

		/* Assume that when day becomes negative, there will certainly
		 * be overflow on seconds.
		 * The check for overflow needs not to be done for leapyears
		 * divisible by 400.
		 * The code only works when year (1970) is not a leapyear.
		 */

		tm_year = tm->tm_year + YEAR0;

		//if (LONG_MAX / 365 < tm_year - year)
		//	overflow++;
		day = (tm_year - year) * 365;
		//				if (LONG_MAX - day < (tm_year - year) / 4 + 1)
		//					overflow++;
		day += (tm_year - year) / 4 + ((tm_year % 4) && tm_year % 4 < year % 4);
		day -= (tm_year - year) / 100 + ((tm_year % 100) && tm_year % 100 < year % 100);
		day += (tm_year - year) / 400 + ((tm_year % 400) && tm_year % 400 < year % 400);

		yday = month = 0;
		switch (tm->tm_mon)
		{
		case 0:
			break;
		case 1:
			yday += 31;
			break;
		case 2:
			yday += 59 + LEAPYEAR(tm_year);
			break;
		case 3:
			yday += 90 + LEAPYEAR(tm_year);
			break;
		case 4:
			yday += 120 + LEAPYEAR(tm_year);
			break;
		case 5:
			yday += 151 + LEAPYEAR(tm_year);
			break;
		case 6:
			yday += 181 + LEAPYEAR(tm_year);
			break;
		case 7:
			yday += 212 + LEAPYEAR(tm_year);
			break;
		case 8:
			yday += 243 + LEAPYEAR(tm_year);
			break;
		case 9:
			yday += 273 + LEAPYEAR(tm_year);
			break;
		case 10:
			yday += 304 + LEAPYEAR(tm_year);
			break;
		case 11:
			yday += 334 + LEAPYEAR(tm_year);
			break;
		}

		yday += (tm->tm_mday - 1);
		//				if (day + yday < 0)
		//					overflow++;
		day += yday;

		tm->tm_yday = yday;
		tm->tm_wday = (day + 4)%7; /* day 0 was thursday (4) */

		seconds = ((tm->tm_hour * 60L) + tm->tm_min) * 60L + tm->tm_sec;

		//				if ((TIME_MAX - seconds) / SECS_DAY < day)
		//					overflow++;
		seconds += day * SECS_DAY;

		/* Now adjust according to timezone and daylight saving time */

		/* Not for us */
#ifdef ELIMINADO

				if (((_timezone > 0) && (TIME_MAX - _timezone < seconds))
						|| ((_timezone < 0) && (seconds < -_timezone)))
					overflow++;
				seconds += _timezone;

				if (tm->tm_isdst < 0)
					dst = _dstget(tm);
				else if (tm->tm_isdst)
					dst = _dst_off;
				else
					dst = 0;

				if (dst > seconds)
					overflow++; // dst is always non-negative
				seconds -= dst;


				if (overflow)
				{
					value = (time_t) -1;
					return;
				}
#endif /* de ELIMINADO */


		if ((time_t) seconds != (signed) seconds)
		{
			value = (time_t) -1;
		}
		else
		{
			value = (time_t) seconds;
		}
	}


	struct tm *getCalendarFromTimeUTC (struct tm *result)
	{
		return (gmtime_r(&value, result));
	}

	void getDateTimeFromTimeUTC (Date *date, Time *time)
	{
		struct tm timeCalendar;

		gmtime_r(&value,&timeCalendar);

		date->year.value = timeCalendar.tm_year - 100;
		date->month.value = timeCalendar.tm_mon + 1;
		date->day.value = timeCalendar.tm_mday;
		date->week_day.value = timeCalendar.tm_wday;
		date->week_day_SetAssigned(true);
		date->days_2000 = timeCalendar.tm_yday + 365 * (date->year.value) + (date->year.value - 1)/4;
		date->days_2000_SetAssigned(true);

		time->hour.value = timeCalendar.tm_hour;
		time->minute.value = timeCalendar.tm_min;
		time->seconds.value = timeCalendar.tm_sec;
	}

	void getDateTimeFromTimeUTC (DateComplete *date, Time *time)
	{
		struct tm timeCalendar;

		gmtime_r(&value,&timeCalendar);

		date->year.value = timeCalendar.tm_year - 100;
		date->month.value = timeCalendar.tm_mon + 1;
		date->day.value = timeCalendar.tm_mday;
		date->week_day.value = timeCalendar.tm_wday;
		date->days_2000 = timeCalendar.tm_yday + 365 * (date->year.value) + (date->year.value - 1)/4;

		time->hour.value = timeCalendar.tm_hour;
		time->minute.value = timeCalendar.tm_min;
		time->seconds.value = timeCalendar.tm_sec;
	}

};


} // end of namespace samson
} // end of namespace system

#endif
