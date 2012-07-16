

#ifndef _H_strCompare
#define _H_strCompare

struct strCompare : public std::binary_function<const char*, const char*, bool> 
{

   bool operator() (const char* str1, const char* str2) const
   { 
	  return strcmp(str1, str2) < 0; 
   }
};


#endif
