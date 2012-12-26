/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
/* ****************************************************************************
*
* FILE            common_test.cpp
*
* AUTHOR         Gregorio Escalada
*
* DATE            May 2012
*
* DESCRIPTION
*
* unit testing of the common class in the samson  library
*
*/

#include "samson/common/MemoryCheck.h"

#include "samson/common/MessagesOperations.h"

#include "samson/common/SamsonSetup.h"
#include "samson/common/status.h"

#include "samson/common/Rate.h"

#include "samson/common/Visualitzation.h"

#include "gtest/gtest.h"


//Test void MemoryCheck();
TEST(commonTest, MemoryCheck)
{
    // Better not to set /opt/samson and /var/samson, so init() can get environment variables
    //samson::SamsonSetup::init("/opt/samson", "/var/samson");
    samson::SamsonSetup::init("", "");

    EXPECT_EQ(samson::MemoryCheck(), true) << "Error in MemoryCheck test";

    samson::SamsonSetup::destroy();
}

//Test void MessagesOperations();
TEST(commonTest, MessagesOperations)
{
    // Better not to set /opt/samson and /var/samson, so init() can get environment variables
    //samson::SamsonSetup::init("/opt/samson", "/var/samson");
    samson::SamsonSetup::init("", "");

    EXPECT_EQ(samson::filterName("OTTstream.parse_logs", "OTTstream", ""), true) << "Error in filterName positive test";
    EXPECT_EQ(samson::filterName("OTTstream.parse_logs", "system", ""), false) << "Error in filterName negative test";
    EXPECT_EQ(samson::filterName("samson.OTTstream.parse_logs", "samson", "logs"), true) << "Error in filterName positive test";
    EXPECT_EQ(samson::filterName("samson.OTTstream.parse_logs", "samson", "log"), false) << "Error in filterName negative test";

    samson::SamsonSetup::destroy();
}


//Test void SamsonSetup();
TEST(commonTest, SamsonSetup)
{
    EXPECT_EQ(samson::createDirectory("/tmp/testSamsonSetup"), samson::OK) << "Error in createDirectory test";
    EXPECT_EQ(samson::createFullDirectory("/tmp/level1/level2/level3/testSamsonSetup"), samson::OK) << "Error in createFullDirectory test";
    EXPECT_EQ(samson::cannonical_path("/tmp/level1/level2/level3/"), "/tmp/level1/level2/level3") << "Error in cannonical_path test";

    // Better not to set /opt/samson and /var/samson, so init() can get environment variables
    //samson::SamsonSetup::init("/opt/samson", "/var/samson");
    samson::SamsonSetup::init("", "");

    EXPECT_EQ(samson::SamsonSetup::shared()->getValueForParameter("isolated.timeout"), "300") << "Error in getValueForParameter for isolated.timeout";

    samson::SamsonSetup::shared()->addItem("unit_test.samsonSetupTest", "initial", "dummy for testing", samson::SetupItem_string);

    EXPECT_EQ(samson::SamsonSetup::shared()->isParameterDefined("unit_test.samsonSetupTest"), true) << "Error in isParameterDefined positive test";
    EXPECT_EQ(samson::SamsonSetup::shared()->getValueForParameter("unit_test.samsonSetupTest"), "initial") << "Error in getValueForParameter for unit_test.samsonSetupTest";


    EXPECT_EQ(samson::SamsonSetup::shared()->setValueForParameter("unit_test.samsonSetupTest", "successful"), true) << "Error in setValueForParameter for unit_test.samsonSetupTest";
    EXPECT_EQ(samson::SamsonSetup::shared()->getValueForParameter("unit_test.samsonSetupTest"), "successful") << "Error in getValueForParameter for unit_test.samsonSetupTest";

    EXPECT_EQ(samson::SamsonSetup::shared()->setValueForParameter("isolated.timeout", "1000"), true) << "Error in setValueForParameter for isolated.timeout";
    EXPECT_EQ(samson::SamsonSetup::shared()->getValueForParameter("isolated.timeout"), "1000") << "Error in getValueForParameter for isolated.timeout";

    EXPECT_EQ(samson::SamsonSetup::shared()->isParameterDefined("non_existing_entry"), false) << "Error in isParameterDefined for non_existing_entry";
    EXPECT_EQ(samson::SamsonSetup::shared()->getValueForParameter("non_existing_entry"), "Error") << "Error in getValueForParameter for non_existing_entry";


    samson::SamsonSetup::shared()->resetToDefaultValues();

    EXPECT_EQ(samson::SamsonSetup::shared()->isParameterDefined("unit_test.samsonSetupTest"), true) << "Error in isParameterDefined negative test";
    EXPECT_EQ(samson::SamsonSetup::shared()->getValueForParameter("unit_test.samsonSetupTest"), "initial") << "Error in getValueForParameter for unit_test.samsonSetupTest";

    EXPECT_EQ(samson::SamsonSetup::shared()->getValueForParameter("isolated.timeout"), "300") << "Error in getValueForParameter for isolated.timeout";

    samson::SamsonSetup::shared()->clearCustumValues();
    EXPECT_EQ(samson::SamsonSetup::shared()->save(), 0) << "Error in save SamsonSetup";

    samson::SamsonSetup::destroy();
}

//Test  Rate;
TEST(commonTest, Rate)
{

    samson::Rate rate;

    rate.push(1, 10);
    rate.push(1, 10);
    rate.push(1, 10);

    EXPECT_EQ(rate.get_total_kvs(), 3) << "Error in get_total_kvs";
    EXPECT_EQ(rate.get_total_size(), 30) << "Error in get_total_size";

    EXPECT_GT(rate.get_rate_size(), 0) << "Error in get_rate_size";
    EXPECT_GT(rate.get_rate_kvs(), 0) << "Error in get_rate_kvs";
}

//Test  status;
TEST(commonTest, status)
{

    EXPECT_EQ(strcmp(samson::status(samson::OK),"OK") == 0, true) << "Error in status OK";
    EXPECT_EQ(strcmp(samson::status(samson::NotImplemented), "Not Implemented") == 0, true) << "Error in status NotImplemented";
    EXPECT_EQ(strcmp(samson::status(samson::BadMsgType), "BadMsgType") == 0, true) << "Error in status BadMsgType";
    EXPECT_EQ(strcmp(samson::status(samson::NullHost), "Null Host") == 0, true) << "Error in status NullHost";
    EXPECT_EQ(strcmp(samson::status(samson::BadHost), "Bad Host") == 0, true) << "Error in status BadHost";
    EXPECT_EQ(strcmp(samson::status(samson::NullPort), "Null Port") == 0, true) << "Error in status NullPort";
    EXPECT_EQ(strcmp(samson::status(samson::Duplicated), "Duplicated") == 0, true) << "Error in status Duplicated";
    EXPECT_EQ(strcmp(samson::status(samson::KillError), "Kill Error") == 0, true) << "Error in status KillError";
    EXPECT_EQ(strcmp(samson::status(samson::NotHello), "Not Hello") == 0, true) << "Error in status NotHello";
    EXPECT_EQ(strcmp(samson::status(samson::NotAck), "Not an Ack") == 0, true) << "Error in status NotAck";
    EXPECT_EQ(strcmp(samson::status(samson::NotMsg), "Not a Msg") == 0, true) << "Error in status NotMsg";
    EXPECT_EQ(strcmp(samson::status(samson::Error), "Error") == 0, true) << "Error in status Error";
    EXPECT_EQ(strcmp(samson::status(samson::ConnectError), "Connect Error") == 0, true) << "Error in status ConnectError";
    EXPECT_EQ(strcmp(samson::status(samson::AcceptError), "Accept Error") == 0, true) << "Error in status AcceptError";
    EXPECT_EQ(strcmp(samson::status(samson::NotListener), "Not a Listener") == 0, true) << "Error in status NotListener";
    EXPECT_EQ(strcmp(samson::status(samson::SelectError), "Select Error") == 0, true) << "Error in status SelectError";
    EXPECT_EQ(strcmp(samson::status(samson::SocketError), "Socket Error") == 0, true) << "Error in status SocketError";
    EXPECT_EQ(strcmp(samson::status(samson::GetHostByNameError), "Get Host By Name Error") == 0, true) << "Error in status GetHostByNameError";
    EXPECT_EQ(strcmp(samson::status(samson::BindError), "Bind Error") == 0, true) << "Error in status BindError";
    EXPECT_EQ(strcmp(samson::status(samson::ListenError), "Listen Error") == 0, true) << "Error in status ListenError";
    EXPECT_EQ(strcmp(samson::status(samson::ReadError), "Read Error") == 0, true) << "Error in status ReadError";
    EXPECT_EQ(strcmp(samson::status(samson::WriteError), "Write Error") == 0, true) << "Error in status WriteError";
    EXPECT_EQ(strcmp(samson::status(samson::Timeout), "Timeout") == 0, true) << "Error in status Timeout";
    EXPECT_EQ(strcmp(samson::status(samson::ConnectionClosed), "Connection Closed") == 0, true) << "Error in status ConnectionClosed";
    EXPECT_EQ(strcmp(samson::status(samson::PThreadError), "Thread Error") == 0, true) << "Error in status PThreadError";
    EXPECT_EQ(strcmp(samson::status(samson::WrongPacketHeader), "Wrong Packet Header") == 0, true) << "Error in status WrongPacketHeader";
    EXPECT_EQ(strcmp(samson::status(samson::ErrorParsingGoogleProtocolBuffers), "Error parsing Google Protocol Buffer message") == 0, true) << "Error in status ErrorParsingGoogleProtocolBuffers";

    EXPECT_EQ(samson::au_status(au::OK), samson::OK) << "Error in au_status OK";
    EXPECT_EQ(samson::au_status(au::Error), samson::Error) << "Error in au_status Error";
    EXPECT_EQ(samson::au_status(au::NotImplemented), samson::NotImplemented) << "Error in au_status NotImplemented";
    EXPECT_EQ(samson::au_status(au::GetHostByNameError), samson::GetHostByNameError) << "Error in au_status GetHostByNameError";
    EXPECT_EQ(samson::au_status(au::SelectError), samson::Timeout) << "Error in au_status SelectError";
    EXPECT_EQ(samson::au_status(au::Timeout), samson::Timeout) << "Error in au_status Timeout";
    EXPECT_EQ(samson::au_status(au::ConnectError), samson::ConnectError) << "Error in au_status ConnectError";
    EXPECT_EQ(samson::au_status(au::AcceptError), samson::AcceptError) << "Error in au_status AcceptError";
    EXPECT_EQ(samson::au_status(au::SocketError), samson::SocketError) << "Error in au_status SocketError";
    EXPECT_EQ(samson::au_status(au::BindError), samson::BindError) << "Error in au_status BindError";
    EXPECT_EQ(samson::au_status(au::ListenError), samson::ListenError) << "Error in au_status ListenError";
    EXPECT_EQ(samson::au_status(au::ReadError), samson::ReadError) << "Error in au_status ReadError";
    EXPECT_EQ(samson::au_status(au::WriteError), samson::WriteError) << "Error in au_status WriteError";
    EXPECT_EQ(samson::au_status(au::ConnectionClosed), samson::ConnectError) << "Error in au_status ConnectionClosed";
    EXPECT_EQ(samson::au_status(au::OpenError), samson::Error) << "Error in au_status OpenError";
    EXPECT_EQ(samson::au_status(au::GPB_Timeout), samson::Error) << "Error in au_status GPB_Timeout";
    EXPECT_EQ(samson::au_status(au::GPB_ClosedPipe), samson::Error) << "Error in au_status GPB_ClosedPipe";
    EXPECT_EQ(samson::au_status(au::GPB_ReadError), samson::Error) << "Error in au_status GPB_ReadError";
    EXPECT_EQ(samson::au_status(au::GPB_CorruptedHeader), samson::Error) << "Error in au_status GPB_CorruptedHeader";
    EXPECT_EQ(samson::au_status(au::GPB_WrongReadSize), samson::Error) << "Error in au_status GPB_WrongReadSize";
    EXPECT_EQ(samson::au_status(au::GPB_ReadErrorParsing), samson::Error) << "Error in au_status GPB_ReadErrorParsing";
    EXPECT_EQ(samson::au_status(au::GPB_NotInitializedMessage), samson::Error) << "Error in au_status GPB_NotInitializedMessage";
    EXPECT_EQ(samson::au_status(au::GPB_WriteErrorSerializing), samson::Error) << "Error in au_status GPB_WriteErrorSerializing";
    EXPECT_EQ(samson::au_status(au::GPB_WriteError), samson::Error) << "Error in au_status GPB_WriteError";
}

//Test  Visualization;
TEST(commonTest, Visualization)
{
    EXPECT_EQ(samson::match("*", "string"), true) << "Error in match wildcard";
    EXPECT_EQ(samson::match("[A-Z][A-Z]*", "TEST"), true) << "Error in match capital";
    EXPECT_EQ(samson::match("[A-Z][A-Z]*", "test"), false) << "Error in match small";
}

#include "samson/common/KVRange.h"

//Test  KVRange;
TEST(commonTest, testKVRange)
{
    samson::KVRange range_1;
    EXPECT_EQ(range_1.isValid(), true) << "Error in KVRange isValid() for default constructor";
    range_1.set(0, 10);
    EXPECT_EQ(range_1.isValid(), true) << "Error in KVRange isValid() for set operation";
    range_1.set(-1, 10);
    EXPECT_EQ(range_1.isValid(), false) << "Error in KVRange isValid() for wrong set operation";
    range_1.set(0, -1);
    EXPECT_EQ(range_1.isValid(), false) << "Error in KVRange isValid() for wrong set operation";
    range_1.set(1, 0);
    EXPECT_EQ(range_1.isValid(), false) << "Error in KVRange isValid() for wrong set operation";
    range_1.set(0, 20);

    samson::KVRange range_2(1, 10);
    EXPECT_EQ(range_1.isValid(), true) << "Error in KVRange isValid() for initialised constructor";

    std::ostringstream output;
    range_2.getInfo(output);
    EXPECT_EQ(output.str(), "<kv_range><hg_begin><![CDATA[1]]></hg_begin><hg_end><![CDATA[10]]></hg_end></kv_range>") << "Error in KVRange getInfo";

    EXPECT_EQ(range_2.str(), "[1 10]") << "Error in KVRange str";
    EXPECT_EQ(range_2.getNumHashGroups(), 9) << "Error in KVRange getNumHashGroups";

    EXPECT_EQ(range_1.overlap(range_2), true) << "Error in KVRange overlap true";
    EXPECT_EQ(range_2.overlap(range_1), true) << "Error in KVRange overlap true";
    samson::KVRange range_3(10, 30);
    EXPECT_EQ(range_3.overlap(range_1), true) << "Error in KVRange overlap true";
    EXPECT_EQ(range_3.overlap(range_2), false) << "Error in KVRange overlap false";

    EXPECT_EQ(range_1.includes(range_2), true) << "Error in KVRange includes true";
    EXPECT_EQ(range_2.includes(range_1), false) << "Error in KVRange includes false";
    EXPECT_EQ(range_1.contains(15), true) << "Error in KVRange contains true";
    EXPECT_EQ(range_1.contains(20), false) << "Error in KVRange contains false";
    EXPECT_EQ(range_1.contains(range_2), true) << "Error in KVRange includes true";
    EXPECT_EQ(range_2.contains(range_1), false) << "Error in KVRange includes false";
    EXPECT_EQ(range_1.isValidForNumDivisions(10), true) << "Error in KVRange isValidForNumDivisions true";
    EXPECT_EQ(range_1.isValidForNumDivisions(20000), false) << "Error in KVRange isValidForNumDivisions false";
    EXPECT_EQ(range_1.getMaxNumDivisions(), 2048) << "Error in KVRange getMaxNumDivisions true";
    EXPECT_EQ(range_2.getMaxNumDivisions(), 4096) << "Error in KVRange getMaxNumDivisions false";


    EXPECT_EQ((range_1 < range_2), false) << "Error in KVRange operator < false";
    range_1.set(11,20);
    EXPECT_EQ((range_2 < range_1), true) << "Error in KVRange operator < true";

    EXPECT_EQ((range_1 == range_2), false) << "Error in KVRange operator == false";
    range_1.set(1,10);
    EXPECT_EQ((range_2 == range_1), true) << "Error in KVRange operator == true";

    EXPECT_EQ((range_1 != range_2), false) << "Error in KVRange operator != false";
    range_1.set(1,11);
    EXPECT_EQ((range_2 != range_1), true) << "Error in KVRange operator != true";
}

#include "samson/common/KVInfo.h"

//Test  KVInfo;
TEST(commonTest, testKVInfo)
{
    samson::KVInfo info_1;
    EXPECT_EQ(info_1.str(), "(     0 kvs in     0 bytes )") << "Error in KVInfo str() for default constructor";
    info_1.set(100, 10);
    EXPECT_EQ(info_1.str(), "(  10.0 kvs in   100 bytes )") << "Error in KVInfo str() for set";

    std::ostringstream output;
    info_1.getInfo(output);
    EXPECT_EQ(output.str(), "<kv_info><kvs><![CDATA[10]]></kvs><size><![CDATA[100]]></size></kv_info>") << "Error in KVInfo getInfo";


    info_1.append(1, 1);
    EXPECT_EQ(info_1.str(), "(  11.0 kvs in   101 bytes )") << "Error in KVInfo str() for append";
    info_1.remove(2, 2);
    EXPECT_EQ(info_1.str(), "(  9.00 kvs in  99.0 bytes )") << "Error in KVInfo str() for append";
    EXPECT_EQ(info_1.isEmpty(), false) << "Error in KVInfo isValid() for isEmpty() false";
    info_1.clear();
    EXPECT_EQ(info_1.isEmpty(), true) << "Error in KVInfo isValid() for isEmpty() true";

    samson::KVInfo info_2(2000, 20);
    EXPECT_EQ(info_2.str(), "(  20.0 kvs in  2.00Kbytes )") << "Error in KVInfo for initialised constructor";
    EXPECT_EQ(info_1.canAppend(info_2), true) << "Error in KVInfo str() for canAppend";
    info_1.append(info_2);
    EXPECT_EQ(info_1.str(), "(  20.0 kvs in  2.00Kbytes )") << "Error in KVInfo append from KVInfo";

}

#include "samson/common/FullKVInfo.h"
#include "samson/common/common.h"

//Test  FullKVInfo;
TEST(commonTest, testFullKVInfo)
{
    samson::FullKVInfo info_1;
    EXPECT_EQ(info_1.str(), "(     0 kvs in     0 bytes )") << "Error in FullKVInfo isValid() for default constructor";
    info_1.set(static_cast<samson::uint32>(100), static_cast<samson::uint32>(10));
    EXPECT_EQ(info_1.str(), "(  10.0 kvs in   100 bytes )") << "Error in FullKVInfo isValid() for set";

    std::ostringstream output;
    info_1.getInfo(output);
    EXPECT_EQ(output.str(), "<kv_info><kvs><![CDATA[10]]></kvs><size><![CDATA[100]]></size></kv_info>") << "Error in FullKVInfo getInfo";


    info_1.append(static_cast<samson::uint32>(1), static_cast<samson::uint32>(1));
    EXPECT_EQ(info_1.str(), "(  11.0 kvs in   101 bytes )") << "Error in FullKVInfo isValid() for append";
    info_1.remove(static_cast<samson::uint32>(2), static_cast<samson::uint32>(2));
    EXPECT_EQ(info_1.str(), "(  9.00 kvs in  99.0 bytes )") << "Error in FullKVInfo isValid() for append";
    EXPECT_EQ(info_1.isEmpty(), false) << "Error in FullKVInfo isValid() for isEmpty false";
    info_1.clear();
    EXPECT_EQ(info_1.isEmpty(), true) << "Error in FullKVInfo isValid() for isEmpty true";

    samson::FullKVInfo info_2(2000, 20);
    EXPECT_EQ(info_2.str(), "(  20.0 kvs in  2.00Kbytes )") << "Error in FullKVInfo for initialised constructor";
    info_1.append(info_2);
    EXPECT_EQ(info_1.str(), "(  20.0 kvs in  2.00Kbytes )") << "Error in FullKVInfo append from KVInfo";

    info_1.set(static_cast<samson::uint64>(1000), static_cast<samson::uint64>(100));
    EXPECT_EQ(info_1.str(), "(   100 kvs in  1.00Kbytes )") << "Error in FullKVInfo isValid() for set 64 bits";
    EXPECT_EQ(info_1.fitsInKVInfo(), true) << "Error in FullKVInfo fitsInKVInfo true";
    samson::uint64 kvs = 1024 * 1024;
    kvs *= 1024 * 8;
    samson::uint64 size = 1024 * 1024;
    size *= 1024 * 8;
    info_1.set(kvs, size);
    EXPECT_EQ(info_1.fitsInKVInfo(), false) << "Error in FullKVInfo fitsInKVInfo false";


}



#include "samson/common/KVFile.h"

//Test  KVFile;
TEST(commonTest, KVFile)
{
    samson::KVFile null_kvfile( NULL );
    EXPECT_EQ(null_kvfile.getErrorMessage(), "NULL _data") << "Error in KVFile constructor for null data";

    // Stupid deprecated conversion from string constant to "char*" work-around
    char *wrong_header= strdup("data without header");
    samson::KVFile wrong_header_kvfile( wrong_header );
    EXPECT_EQ(wrong_header_kvfile.getErrorMessage(), "KVHeader error: wrong magic number") << "Error in KVFile constructor for wrong_header";
    free(wrong_header);
}

#include "samson/common/KVInputVector.h"

//Test  KVInputVector;
TEST(commonTest, KVInputVector)
{
    samson::KVInputVector kv_empty_vector(0);
    samson::KVInputVector kv_old_vector(1);

//    samson::Operation  red_op("test_reduce_operation", samson::Operation::reduce);
//    samson::KVInputVector kv_red_op_vector(&red_op);
//
//    samson::Operation  map_op("test_map_operation", samson::Operation::map);
//    samson::KVInputVector kv_map_op_vector(&map_op);
//
//    EXPECT_EQ(kv_empty_vector.getNext(), (samson::KVSetStruct *)NULL) << "Error in getNext for empty KVInputVector";
//    EXPECT_EQ(kv_old_vector.getNext(), (samson::KVSetStruct *)NULL) << "Error in getNext for kv_old_vector KVInputVector";
//    EXPECT_EQ(kv_red_op_vector.getNext(), (samson::KVSetStruct *)NULL) << "Error in getNext for kv_red_op_vector KVInputVector";
//    EXPECT_EQ(kv_map_op_vector.getNext(), (samson::KVSetStruct *)NULL) << "Error in getNext for kv_map_op_vector KVInputVector";
//
//    kv_empty_vector.sort();
//    kv_red_op_vector.sort();




}


