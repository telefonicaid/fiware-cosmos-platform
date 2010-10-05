#ifndef SAMSON_H
#define SAMSON_H

/* ****************************************************************************
*
* FILE                  samson.h
*
*     Created by ANDREU URRUELA PLANAS on 7/27/10.
*     Copyright 2010 TID. All rights reserved.
*
* SAMSON is a platform for stream-oriented process of data sources.
* It is specially designed to process high-volumen sources with stream-map&reduce operations
*
* Everything is optimized to perform as much as possible all operations in memory.
* At the same time, SAMSON is a map&reduce distributed platform based on MACRO
* (previous version of the map&reduce platform of Telefónica I+D)
* 
* Architecture:
*
* Network Manager (SSNetworkManager) is the component to interacto with other nodes and with the exterior
*
* Memory Manager  (SSNetworkManager) is the component to manage memory of a samson-node.
*                 It offers a threat-safe quick API to obtain memory blocks (KVBlock)
*                 It is responsible for backing up everything on disk as needed and put back in memory
* 
* Task Manager    (TaskManager) is the component responsible to run all process operations
*                 Is is responsible to keep a list of tasks with dependencies that can run independently in all available cores
*	
* KV Manager      (KVManager) is the component to manage the KV-Storages, KV-Sets and in general all data elements
* 
* SAMSON Manager  (SSManager) is the general component to manage a samson node
*/
#include "BufferSizes.h"  /* AU_SIZE_100M */
 


// Provisional constants
#define MAX_SIZE_KEY_VALUE		AU_SIZE_100M			// Maximum size of the key-value pair (for streaming reasons)

// Default directories to work

#define SAMSON_BASE_DIRECTORY			"/var/samson"
#define SAMSON_WORKSPACE_DIRECTORY		"/var/samson/workspace"
#define SAMSON_LOG_DIRECTORY			"/var/samson/log"
#define SAMSON_DATA_DIRECTORY			"/var/samson/data"
#define SAMSON_MODULES_DIRECTORY		"/var/samson/modules"


#define SAMSON_MAX_KVSET_SIZE			16*1024*1024
#define NUM_SETS_PER_STORAGE			10
#define SAMSON_DEFAULT_MEMORY			"2G"
#define SAMSON_DEFAULT_CORES			2

/**
 As a rule of thumb, memory >> cores * num_storages * max_kv_set 
*/


// #define DEBUG_TASK_MANAGER

#endif
