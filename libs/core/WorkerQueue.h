#ifndef _H_WORKER_QUEUE
#define _H_WORKER_QUEUE


namespace ss {

	class WorkerQueue
	{
		std::string _name;					// Name of this queue
		KVFormat _format;					// Format of this queue
		KVInfo info;						// Information about this queue 
		
		// List of files containing data for this queue
		// TODO: Review how to store this stuff
		std::vector<std::string> fileName;
		
	public:
		WorkerQueue( std::string name , KVFormat format )
		{
			_name = name;
			_format = format;
		}
		
		std::string str()
		{
			std::ostringstream o;
			o << "(" << _format.str() << ") ";
			o << info.str();
			return o.str();		
		}
		
		void addFile(std::string _fileName )
		{
			fileName.push_back(_fileName);
		}
		
		void clear()
		{
			fileName.clear();
		}
		
		void rename( std::string name )
		{
			_name = name;
		}
		
		
		KVInfo getInfo()
		{
			return info;
		}
	};	
	

}

#endif