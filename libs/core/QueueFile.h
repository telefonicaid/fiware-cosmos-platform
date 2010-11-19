
#ifndef _H_QUEUE_FILE
#define _H_QUEUE_FILE

namespace ss {

	class QueueFile
	{
		std::string fileName;
		int worker;
		KVInfo info;
		
	public:
		
		QueueFile( std::string& _fileName , int _worker , KVInfo _info )
		{
			fileName = _fileName;
			worker = _worker;
			info = _info;
		}
		
	};
	
}

#endif
