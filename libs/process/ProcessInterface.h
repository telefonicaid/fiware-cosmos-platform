#ifndef _H_PROCESS_INTERFACE
#define _H_PROCESS_INTERFACE

namespace ss {
	
	class ProcessInterface
	{
	public:
		
		ProcessInterface()
		{
		}
		
		virtual char* passCommand(const char* command)=0;

		
	};
}

#endif
