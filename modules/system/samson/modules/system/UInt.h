
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this data
 */

#ifndef _H_SAMSON_system_UInt
#define _H_SAMSON_system_UInt


#undef DEBUG_FILES
#ifdef DEBUG_FILES
#include <iostream>
#include <fstream>
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES


namespace samson{
namespace system{


/**
		 UInt  Data Instance 
 */

class UInt : public samson::DataInstance {

public:

	size_t value;

	UInt() : samson::DataInstance(){
	}

	~UInt() {
	}

	std::string getName()
	{
	   return "system.UInt";
	}
	
	int parse(char *data){
		return samson::staticVarIntParse( data , &value);
	}

	int serialize(char *data){
		return samson::staticVarIntSerialize( data , value);
	}

	int hash(int max_num_partitions){
		return value%max_num_partitions;
	}


	static int size(char *data){
		size_t _value;
		return samson::staticVarIntParse( data , &_value);
	}

	int toInt()
	{
		return (value);
	}

	inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
		{ // comparing b
			size_t _value1;
		size_t _value2;
		*offset1 += samson::staticVarIntParse(data1 + (*offset1), &_value1);
		*offset2 += samson::staticVarIntParse(data2 + (*offset2), &_value2);
		if( _value1 < _value2 ) return -1;
		if( _value1 > _value2 ) return  1;
		return 0;
		}
		return 0; //If everything is equal
	}

	inline static int compare( char* data1 , char* data2 )
	{
		size_t offset_1=0;
		size_t offset_2=0;
		return compare( data1 , data2 , &offset_1 , &offset_2 );
	}

    int serial_compare( char* data1 , char* data2 )
    {
       return compare( data1, data2 );
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
#undef DEBUG_FILES
#ifdef DEBUG_FILES
		{
			std::string filename = "/tmp/debug_UInt.log";
			std::ofstream fs(filename.c_str(), std::ios::app);
			fs << "Calling getDataPath with dataPathCharP: '" << dataPathCharP << "'" << std::endl;
			fs.close();
		}
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES

		if (*dataPathCharP == 0)
		{
			*dataPathIntP = -1;
			return (0);
		}

		if (strcmp(dataPathCharP, "UInt") == 0)
		{
			*dataPathIntP = -1;
			return (0);
		}

		return -1;
	}

	std::string getTypeFromPath(const std::string &dataPathString){
		const char *dataPathCharP = dataPathString.c_str();
#undef DEBUG_FILES
#ifdef DEBUG_FILES
		{
			std::string filename = "/tmp/debug_UInt.log";
			std::ofstream fs(filename.c_str(), std::ios::app);
			fs << "Calling getTypeFromPath with dataPathString: '" << dataPathString << "'" << std::endl;
			fs.close();
		}
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES

		return(getTypeFromPathStatic(dataPathCharP));
	}

	static std::string getTypeFromPathStatic(const char * dataPathCharP){
#undef DEBUG_FILES
#ifdef DEBUG_FILES
		{
			std::string filename = "/tmp/debug_UInt.log";
			std::ofstream fs(filename.c_str(), std::ios::app);
			fs << "Calling getTypeFromPathStatic with dataPathCharP: '" << dataPathCharP << "'" << std::endl;
			fs.close();
		}
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES

		if (*dataPathCharP == 0)
		{
			return ("system.UInt");
		}
		if (strcmp(dataPathCharP, ".") == 0)
		{
			return ("system.UInt");
		}

		if (strcmp(dataPathCharP, "UInt") == 0)
		{
			return ("system.UInt");
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
#undef DEBUG_FILES
#ifdef DEBUG_FILES
		{
			std::string filename = "/tmp/debug_UInt.log";
			std::ofstream fs(filename.c_str(), std::ios::app);
			fs << "In  getTypeFromPathStatic found: '" << "system.UInt" << "'" << std::endl;
			fs.close();
		}
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES

		return ("system.UInt");
		break;
		default:
			return ("_ERROR_");
			break;
		};
	}

	static const char *getTypeStatic()
	{
		return ("system.UInt");
	}

	const char *getType()
	{
		return ("system.UInt");
	}

	static bool checkTypeStatic(const char *type)
	{
		if (strcmp(type, "system.UInt") == 0)
		{
			return true;
		}
		return false;
	}

	bool checkType(const char *type)
	{
		if (strcmp(type, "system.UInt") == 0)
		{
			return true;
		}
		return false;
	}

	static size_t getHashTypeStatic(){
		return(1219561887489248778ULL);
	}

	size_t getHashType(){
		return(1219561887489248778ULL);
	}

	static bool checkHashTypeStatic(size_t valType){
		if (valType == 1219561887489248778ULL)
		{
			return true;
		}		return false;
	}

	 bool checkHashType(size_t valType){
		if (valType == 1219561887489248778ULL)
		{
			return true;
		}		return false;
	}


	DataInstance * getDataInstanceFromPath(const int *dataPathIntP){
#undef DEBUG_FILES
#ifdef DEBUG_FILES
		{
			std::string filename = "/tmp/debug_UInt.log";
			std::ofstream fs(filename.c_str(), std::ios::app);
			fs << "Calling getDataInstanceFromPath with *dataPathIntP:" << *dataPathIntP << std::endl;
			fs.close();
		}
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES

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

	void copyFrom( UInt *other ){
		value = other->value;
	};

	std::string str(){
		std::ostringstream o;
		o << value;
		return o.str();
	}
	/*
			void operator= (int _value) {
				value = _value;
			}			
	 */

	void operator++ () {
		value++;
	}

	// Comparation with size_t

	void operator= (size_t _value) {
		value = _value;
	}

	bool operator< (size_t _value) {
		return ( value < _value);
	}

	bool operator> (size_t _value) {
		return ( value > _value);
	}

	bool operator<= (size_t _value) {
		return ( value <= _value);
	}

	bool operator>= (size_t _value) {
		return ( value >= _value);
	}

	bool operator== (size_t _value) {
		return ( value == _value);
	}

	bool operator!= (size_t _value) {
		return ( value != _value);
	}

	UInt& operator+=(size_t _value) {
		value+=_value;
		return *this;
	}

	UInt& operator-=(size_t _value) {
		value-=_value;
		return *this;
	}


	// Self comparison

	UInt& operator+=(const UInt &o) {
		value+=o.value;
		return *this;
	}

	UInt& operator=(const UInt &o) {
		value=o.value;
		return *this;
	}

	bool operator== (UInt& o) {
		return ( value == o.value);
	}

	bool operator!= (UInt& o) {
		return ( value != o.value);
	}

	bool operator< (UInt& o) {
		return ( value < o.value);
	}

	bool operator> (UInt& o) {
		return ( value > o.value);
	}

	bool operator<= (UInt& o) {
		return ( value <= o.value);
	}

	bool operator>= (UInt& o) {
		return ( value >= o.value);
	}


};


} // end of namespace samson
} // end of namespace system

#endif
