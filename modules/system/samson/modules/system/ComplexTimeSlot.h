
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this data
 */

#ifndef _H_SAMSON_system_ComplexTimeSlot
#define _H_SAMSON_system_ComplexTimeSlot


#include <samson/modules/system/ComplexTimeSlot_base.h>
#include <samson/modules/system/Date.h>
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


class ComplexTimeSlot : public ComplexTimeSlot_base
{

public:
	/**
	 * Method to fill the array of simple time
	 * slots with the values from the configuration
	 * parameters.
	 *
	 * @param str String that contains the definition
	 *    of the complex time slot, with the format:
	 *    "N|TS1|...|TSN", being N the number of simple
	 *    time slots, and TSi each of them in the format:
	 *    "bbbbbbb hh:mm:ss hh:mm:ss".
	 */
	void set( std::string str )
	{
		TimeSlot *_ts;
		unsigned int nts;

		//Goyo. Repasar cómo se pone
		flagSet.value = false;

		// fill members only the first time
		if( flagSet.value == false )
		{

#undef DEBUG_FILES
#ifdef DEBUG_FILES
                {
                        std::string filename = "/tmp/debug_ComplexTimeSlot.log";
                        std::ofstream fs(filename.c_str(), std::ios::app);
                        fs << "Process set: '" << str << "'" << std::endl;
                        fs.close();
                }
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES


			size_t _pos = 0;
			std::string _strTs = "";
			std::string _tmp = "";
			unsigned char _wdaysMask;

			_pos = str.find( '|', 0 );
			if( _pos > 0 )
			{
				nts = atoi( str.substr( 0, _pos ).c_str() );

#undef DEBUG_FILES
#ifdef DEBUG_FILES
                {
                        std::string filename = "/tmp/debug_ComplexTimeSlot.log";
                        std::ofstream fs(filename.c_str(), std::ios::app);
                        fs << "Detect fields with nts: '" << nts << "'" << std::endl;
                        fs.close();
                }
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES


#undef DEBUG_FILES
#ifdef DEBUG_FILES
                {
                        std::string filename = "/tmp/debug_ComplexTimeSlot.log";
                        std::ofstream fs(filename.c_str(), std::ios::app);
                        fs << "Detect fields with: '" << str.substr( _pos ) << "'" << std::endl;
                        fs.close();
                }
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES

				// check lenght of the remaining substring
				// before inserting timeslot strings
				// into the array
				if( str.substr( _pos ).length() == (26*nts) )
				{
					// initialize total weekdays mask
					wdaysMask = WDAY_NONE;

					_pos++;
					for( int i=0; i<nts; i++ )
					{
						_strTs = str.substr( _pos, 25 );

						_ts = tsAdd();

#undef DEBUG_FILES
#ifdef DEBUG_FILES
                {
                        std::string filename = "/tmp/debug_ComplexTimeSlot.log";
                        std::ofstream fs(filename.c_str(), std::ios::app);
                        fs << "Process set with : '" << _strTs << "'" << std::endl;
                        fs.close();
                }
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES

						_ts->set(_strTs.c_str());

						// set position to next timeslot start
						_pos += 26;
					}

#ifdef MEJOR_FUERA_DEL_TIMESLOT
					// get number of days of the month
					GetMonthDays( &mob_month, &monthDays );

					// get week day of the first day of the month
					ComputeDayOfTheWeek( &mob_month );
					wdayFirstDay = mob_month.week_day;

					// compute number of days covered by the time slot
					setCoveredDays();
#endif // de MEJOR_FUERA_DEL_TIMESLOT
				}
				else
				{
					// error
					nts = 0;
				}

				// set flag in any case
				flagSet = true;
			}
		}
	}


	/**
	 * Method that tests if a certain date
	 * and time is included into the complex
	 * time slot.
	 *
	 * @param date MACRO structure that stores the date.
	 * @param time MACRO structure that stores the time.
	 */
	bool includes( Date *date, Time *time )
	{
		for( int i=0; i<ts_length; i++ )
		{
			if( ts[i].includes( date, time ) == true )
			{
				return true;
			}
		}

		// if this point is reached, the time is not included
		// inside any of the simple timeslots of the complex one
		return false;
	}

	/**
	 * Method that tests if a certain date
	 * and time is included into the complex
	 * time slot.
	 *
	 * @param date MACRO structure that stores the date.
	 * @param time MACRO structure that stores the time.
	 */
	bool includes( TimeUnix *time )
	{
		for( int i=0; i<ts_length; i++ )
		{
			if( ts[i].includes( time ) == true )
			{
				return true;
			}
		}

		// if this point is reached, the time is not included
		// inside any of the simple timeslots of the complex one
		return false;
	}

	/**
	 * Method that computes the week day of the date,
	 * and the number of days in the month (to be used
	 * in computing the covered days
	 */
	void setWeekDayFirstDay(Date *date)
	{
		if (date->week_day_GetAssigned() == false)
		{
			date->compute_day_of_the_week();
		}
		wdayFirstDay.value = date->week_day.value;

		monthDays.value = date->GetMonthDays();
	}


	/**
	 * Method that computes the number of days
	 * covered by the time slot and sets the
	 * corresponding member of the class.
	 */
	void setCoveredDays()
	{
		unsigned char _mask;

		coveredDays.value = 0;

		for( int i=0; i<7; i++ )
		{
			_mask = (unsigned char)pow( 2, i );

			if( (wdaysMask.value & _mask) == _mask )
			{
				coveredDays.value += 4;

				if( i == wdayFirstDay.value )
				{
					if( monthDays.value > 28 ) coveredDays.value++;
				}
				else if( i == ((wdayFirstDay.value+1)%7) )
				{
					if( monthDays.value > 29 ) coveredDays.value++;
				}
				else if( i == ((wdayFirstDay.value+2)%7) )
				{
					if( monthDays.value > 30 ) coveredDays.value++;
				}
			}
		}
	}


	/**
	 * Method that returns the number of
	 * days covered by the time slot.
	 *
	 * @return Value of class member coveredDays.
	 */
	unsigned int getCoveredDays()
	{
		return coveredDays.value;
	}





};


} // end of namespace samson
} // end of namespace system

#endif
