
#ifndef _H_SS_TRACER
#define _H_SS_TRACER

#include <stdarg.h>             /* va_start, va_arg, va_end                  */
#include <stdio.h>


namespace ss
{

	// Interface to send traces to the platform
	
   class Tracer
   {
   public:
	   
      virtual void trace( int channel , const char *trace ) = 0;
   
	   void trace_print(int channel, const char* format, ...)
	   {
		   va_list        args;
		   char           vmsg[1024];
		   char*          allocedString;
		   char*          nl;
		   
		   /* "Parse" the varible arguments */
		   va_start(args, format);
		   
		   /* Print message to variable */
		   vsnprintf(vmsg, sizeof(vmsg), format, args);
		   vmsg[1023] = 0;
		   va_end(args);
		   
		   if ((nl = strchr(vmsg, '\n')) != NULL)
			   *nl = 0;
		   
		   allocedString = (char*) strdup(vmsg);

		   trace(channel, allocedString );
		   
		   free( allocedString );
		   
	   }
	   
	   
   };
	
	
	
}

#endif
