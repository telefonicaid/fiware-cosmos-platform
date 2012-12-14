/* ****************************************************************************
 *
 * FILE            KVFileTest.cpp
 *
 * AUTHOR          Ken Zangelin
 *
 * DATE            Nov 2012
 *
 *
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 *
 * DESCRIPTION
 * unit testing of the common class in the samson  library
 *
 */
#include "gtest/gtest.h"

#include "logMsg/logMsg.h"

#include "au/ErrorManager.h"
#include "au/containers/SharedPointer.h"

#include "engine/Buffer.h"
#include "engine/Engine.h"
#include "engine/MemoryManager.h"

#include "samson/common/KVFile.h"
#include "samson/common/KVHeader.h"
#include "samson/module/Data.h"
#include "samson/module/KV.h"
#include "samson/module/ModulesManager.h"

#include "unitTest/common_engine_test.h"

// -----------------------------------------------------------------------------
// createError - test the KVFile create method (its initial error paths)
//
TEST(samson_common_KVFile, createError) {
  init_engine_test();

  {
    samson::KVHeader      header;
    engine::BufferPointer nullBuffer     = (engine::BufferPointer) NULL;
    engine::BufferPointer smallBuffer    = engine::Buffer::Create("testbuffer",  10);
    engine::BufferPointer badMagicBuffer = engine::Buffer::Create("testbuffer2", 1024);

    header.InitForTxt(500);
    badMagicBuffer->Write(reinterpret_cast<char*>(&header), sizeof(samson::KVHeader));

    au::ErrorManager                   errorManager;
    au::SharedPointer<samson::KVFile>  kvFileOkPointer   = samson::KVFile::create(badMagicBuffer, errorManager);

    EXPECT_TRUE(kvFileOkPointer != NULL) <<
      au::str("kvFileOkPointer == NULL: '%s'", errorManager.GetLastError().c_str());

    au::SharedPointer<samson::KVFile>  kvFileSmallPointer = samson::KVFile::create(smallBuffer, errorManager);
    EXPECT_TRUE(kvFileSmallPointer == NULL) << "samson::KVFile::create should have returned NULL here";

    au::SharedPointer<samson::KVFile>  kvFileNullPointer = samson::KVFile::create(nullBuffer, errorManager);
    EXPECT_TRUE(kvFileNullPointer == NULL) << "samson::KVFile::create should have returned NULL here";
  }

  close_engine_test();
}

// -----------------------------------------------------------------------------
// createOkAndError - test the KVFile create method
//
TEST(DISABLED_samson_common_KVFile, createOkAndError) {
  init_engine_test();

  samson::KVHeader       header;
  engine::BufferPointer  buffer       = engine::Buffer::Create("testbuffer",  40000);
  char                   fileBuf[40000];
  struct stat            statBuf;
  const char*            sampleFilePath = "test_data/sample_samson_file";
  int                    statResult     = stat(sampleFilePath, &statBuf);

  EXPECT_EQ(0, statResult) << au::str("stat(%s): %s", sampleFilePath, strerror(errno));

  int fd = open(sampleFilePath, O_RDONLY);
  EXPECT_TRUE(fd != -1);

  int nb = read(fd, fileBuf, statBuf.st_size);
  EXPECT_EQ(nb, statBuf.st_size);

  bool writeOk = buffer->Write(fileBuf, statBuf.st_size);
  EXPECT_EQ(true, writeOk);

  au::ErrorManager                   errorManager;
  au::SharedPointer<samson::KVFile>  kvFile    = samson::KVFile::create(buffer, errorManager);

  EXPECT_TRUE(kvFile != NULL) << au::str("samson::KVFile::create: %s", errorManager.GetLastError().c_str());

  // We have a correct KVFile, lets use it for auxiliar functions

  samson::KV*      kvPointer = kvFile->kvs_for_hg(0);
  char*            hgData    = kvFile->data_for_hg(0);
  size_t           bufSize   = kvFile->size();
  samson::KVHeader hdr       = kvFile->header();

  EXPECT_TRUE(kvPointer != NULL);
  EXPECT_TRUE(hgData != NULL);
  EXPECT_EQ(0, bufSize);
  EXPECT_EQ(hdr.magic_number, 4652783);

  close_engine_test();
}

// -----------------------------------------------------------------------------
// fileCreateOK - 
//
TEST(DISABLED_samson_common_KVFile, fileCreateOK) {
  init_engine_test();

  {
    au::ErrorManager         errorMgr;
    samson::ModulesManager*  mm = au::Singleton<samson::ModulesManager>::shared();
    mm->AddModulesFromDirectory("test_modules", errorMgr);

    samson::Data*           dataKeyP   = mm->GetData("system.String");
    samson::Data*           dataValueP = mm->GetData("system.UInt");

    EXPECT_TRUE(dataKeyP   != NULL);
    EXPECT_TRUE(dataValueP != NULL);
  
    samson::KVFormat      format("system.String", "system.UInt");
    samson::KVInfo        info(0, 0);
    engine::BufferPointer buffer         = 
       (engine::BufferPointer) engine::Buffer::Create("testbuffer",  10000);
    samson::KVHeader*     header         = reinterpret_cast<samson::KVHeader*>(buffer->data());
    samson::DataInstance* keyInstanceP   = dataKeyP->getInstance();  
    samson::DataInstance* valueInstanceP = dataValueP->getInstance();  
    char*                 dataPosition   = buffer->data();

    EXPECT_TRUE(keyInstanceP != NULL);
    EXPECT_TRUE(valueInstanceP != NULL);

    size_t offset = sizeof(samson::KVHeader);
    for (unsigned int ix = 0; ix < 100; ++ix) {
       keyInstanceP->setFromString(strdup("123sss"));
       valueInstanceP->setFromString(strdup("12"));

       int size = keyInstanceP->serialize(&dataPosition[offset]);
       offset    += size;
       info.size += size;

       size = valueInstanceP->serialize(&dataPosition[offset]);
       offset    += size;
       info.size += size;

       ++info.kvs;
    }

    header->Init(format, info); // To provoke Error: format("system.StringX", "system.UIntY");
    buffer->set_size(info.size + sizeof(samson::KVHeader));

    au::ErrorManager                   errorManager;
    au::SharedPointer<samson::KVFile>  kvFileP = samson::KVFile::create(buffer, errorManager);

    EXPECT_TRUE(kvFileP != NULL);
    EXPECT_FALSE(errorManager.HasErrors());

    samson::KV* hgKeys = kvFileP->kvs_for_hg(0);
    EXPECT_TRUE(hgKeys == NULL);

    char* hgData = kvFileP->data_for_hg(0);
    EXPECT_TRUE(hgData == NULL);

    size_t size = kvFileP->size();
    EXPECT_EQ(size, 789632);

    std::ostringstream stringStream;
    kvFileP->printContent(3, true, stringStream);
    EXPECT_STREQ(stringStream.str().c_str(), "[ hg 27846 ] \"123sss\" 12\n[ hg 27846 ] \"123sss\" 12\n[ hg 27846 ] \"123sss\" 12\n[ hg 27846 ] \"123sss\" 12\n");

    //
    // Making buffer size smaller than expected
    //
    buffer->set_size(info.size + sizeof(samson::KVHeader) - 1);
    kvFileP = samson::KVFile::create(buffer, errorManager);
    EXPECT_TRUE(kvFileP == NULL);
    EXPECT_TRUE(errorManager.HasErrors());
    buffer->set_size(info.size + sizeof(samson::KVHeader));

    //
    // Making buffer size different than data size (shown in header)
    //
    --header->info.size;
    kvFileP = samson::KVFile::create(buffer, errorManager);
    EXPECT_TRUE(kvFileP == NULL);
    EXPECT_TRUE(errorManager.HasErrors());
    ++header->info.size;

    //
    // Force hash groups not to be ordered
    //
    offset = 0;
    info.clear();
    for (unsigned int ix = 0; ix < 100; ++ix) {
       keyInstanceP->setFromString(au::str("%d%d%d", ix+1, ix, ix-1).c_str());
       valueInstanceP->setFromString(strdup("12"));

       int size = keyInstanceP->serialize(&dataPosition[offset]);
       offset    += size;
       info.size += size;

       size = valueInstanceP->serialize(&dataPosition[offset]);
       offset    += size;
       info.size += size;

       ++info.kvs;
    }
    buffer->set_size(info.size + sizeof(samson::KVHeader));    
    header->Init(format, info);

    kvFileP = samson::KVFile::create(buffer, errorManager);
    EXPECT_TRUE(kvFileP == NULL);

    mm->ClearModulesManager();
  }

  close_engine_test();
}

// -----------------------------------------------------------------------------
// fileCreateBadData - 
//
TEST(DISABLED_samson_common_KVFile, fileCreateBadData) {
  init_engine_test();

  {
    au::ErrorManager         errorMgr;
    samson::ModulesManager*  mm = au::Singleton<samson::ModulesManager>::shared();
    mm->AddModulesFromDirectory("test_modules", errorMgr);

    samson::Data*           dataKeyP   = mm->GetData("system.String");
    samson::Data*           dataValueP = mm->GetData("system.UInt");

    EXPECT_TRUE(dataKeyP   != NULL);
    EXPECT_TRUE(dataValueP != NULL);
  
    samson::KVFormat      format("system.String", "system.UInt33");
    samson::KVInfo        info(0, 0);
    engine::BufferPointer buffer         = 
       (engine::BufferPointer) engine::Buffer::Create("testbuffer",  10000);
    samson::KVHeader*     header         = reinterpret_cast<samson::KVHeader*>(buffer->data());
    samson::DataInstance* keyInstanceP   = dataKeyP->getInstance();  
    samson::DataInstance* valueInstanceP = dataValueP->getInstance();  
    char*                 dataPosition   = buffer->data();

    EXPECT_TRUE(keyInstanceP != NULL);
    EXPECT_TRUE(valueInstanceP != NULL);

    size_t offset = sizeof(samson::KVHeader);
    for (unsigned int ix = 0; ix < 100; ++ix) {
       keyInstanceP->setFromString("123sss");
       valueInstanceP->setFromString("12");

       int size = keyInstanceP->serialize(&dataPosition[offset]);
       offset    += size;
       info.size += size;

       size = valueInstanceP->serialize(&dataPosition[offset]);
       offset    += size;
       info.size += size;

       ++info.kvs;
    }

    header->Init(format, info); // To provoke Error: format("system.StringX", "system.UIntY");
    buffer->set_size(info.size + sizeof(samson::KVHeader) + 1);

    au::ErrorManager                   errorManager;
    au::SharedPointer<samson::KVFile>  kvFileP = samson::KVFile::create(buffer, errorManager);

    EXPECT_TRUE(kvFileP == NULL);
    EXPECT_TRUE(errorManager.HasErrors());

    mm->ClearModulesManager();
  }

  close_engine_test();
}

// -----------------------------------------------------------------------------
// printContentIsTxt - test the KVFile printContent method for TXT files
//
TEST(DISABLED_samson_common_KVFile, printContentIsTxt) {
  
}
