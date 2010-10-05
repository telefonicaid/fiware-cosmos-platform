#ifndef SAMSON_KV_SET_MAINTAINER_H
#define SAMSON_KV_SET_MAINTAINER_H

#include <pthread.h>



namespace ss
{
	void runKVSetMaintainer();
	void runAutomaticTasksScheduler();
	void runTasksWorkers(int cores);
	void wakeUpWorkers();
	void runMonitorizationThread();
}

#endif
