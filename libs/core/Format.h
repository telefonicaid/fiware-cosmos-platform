#ifndef SAMSON_FORMAT_H
#define SAMSON_FORMAT_H


namespace au {


class Format
{
	
public:

	static std::string percentage_string( double value , double total )
	{
		if ( total == 0)
			return percentage_string( 0 );
		else
			return percentage_string( value / total);
	}
	
	static std::string percentage_string( double p )
	{
		char line[2000];
		sprintf(line, "%03.1f%%",p*100);
		return std::string(line);
		
	}
	
	static  std::string string( double value, char  letter )
	{
		char line[2000];
		
		if ( value < 10 )
			sprintf(line, "%4.2f%c",value,letter);
		else if ( value < 100 )
			sprintf(line, "%4.1f%c",value,letter);
		else 
			sprintf(line, "%4.0f%c",value,letter);
		
		
		return std::string( line );
	}


	static std::string string( size_t memory )
	{
		
		if (memory < 1000)
			return string( (double)memory , ' ' );
		else if( memory < 10000000)
			return string( (double)memory/ 1000.0 , 'K');
		else if( memory < 1000000000)
			return string( (double)memory/ 1000000.0 , 'M');
		else 
			return string( (double)memory/ 1000000000.0 , 'G');
		
	}

	static std::string int_string(int value, int digits)
	{
		
		char line[100];
		char format[100];
		
		
		sprintf(format, "%%%dd",digits);
		sprintf(line, format,value ); 
		return std::string(line);
	}

	static std::string time_string( int seconds )
	{
		int minutes = seconds/60;
		seconds -= minutes*60;
		
		int hours = minutes/60;
		minutes -= hours*60;
		
		char line[1000];
		sprintf(line, "%02d:%02d:%02d" , hours , minutes , seconds	);
		return std::string(line);
		
	}
};
}

#endif
