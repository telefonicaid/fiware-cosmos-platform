
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this data
*/

#ifndef _H_SAMSON_cdr_Date
#define _H_SAMSON_cdr_Date


#include <samson/modules/cdr/Date_base.h>
#include <iostream>
#include <iomanip>


namespace ss{
namespace cdr{


	class Date : public Date_base
	{
	public:
		bool operator== (Date& date) {
			
			if( day.value	!= date.day.value)		return false;
			if( month.value	!= date.month.value)	return false;
			if( year.value	!= date.year.value)		return false;
			
			return true;
		}
		bool operator!= (Date& date) {
			
			if( day.value	!= date.day.value)		return true;
			if( month.value	!= date.month.value)	return true;
			if( year.value	!= date.year.value)		return true;
			
			return false;
		}
		
		std::string str(){
			std::ostringstream o;
			o << std::setw(2) << std::setfill('0') << (int) day.value;
			o << "/";
			o << std::setw(2) << std::setfill('0') << (int) month.value;
			o << "/";
			o << std::setw(2) << std::setfill('0') << (int) year.value;
			return o.str();
		}
		
		
		void compute_dat_of_the_week()
		{
			tm tm_original;		
			
			tm_original.tm_hour = 12;
			tm_original.tm_min = 0;
			tm_original.tm_sec = 0;
			
			tm_original.tm_mday = day.value;
			tm_original.tm_mon = month.value - 1;	// Range 0-11
			
			if( year.value < 64 )
				tm_original.tm_year = 2000 + year.value - 1900;
			else
				tm_original.tm_year = 1900 + year.value - 1900;
			
			time_t t = mktime(&tm_original);
			
			tm result;
			gmtime_r(&t, &result);
			
			if( result.tm_wday == 0)
				week_day.value = 6;
			else
				week_day.value = result.tm_wday-1;
		}
		
		
		
		
	};


} // end of namespace ss
} // end of namespace cdr

#endif
