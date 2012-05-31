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
    samson::SamsonSetup::init("/opt/samson", "/var/samson");

    EXPECT_EQ(samson::MemoryCheck(), true) << "Error in MemoryCheck test";

    samson::SamsonSetup::destroy();
}

//Test void MessagesOperations();
TEST(commonTest, MessagesOperations)
{
    samson::SamsonSetup::init("/opt/samson", "/var/samson");

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

    samson::SamsonSetup::init("/opt/samson", "/var/samson");

    EXPECT_EQ(samson::SamsonSetup::shared()->getValueForParameter("isolated.timeout"), "300") << "Error in getValueForParameter for isolated.timeout";

    samson::SamsonSetup::shared()->addItem("unit_test.samsonSetupTest", "initial", "dummy for testing", samson::SetupItem_string);

    EXPECT_EQ(samson::SamsonSetup::shared()->isParameterDefined("unit_test.samsonSetupTest"), true) << "Error in isParameterDefined positive test";
    EXPECT_EQ(samson::SamsonSetup::shared()->getValueForParameter("unit_test.samsonSetupTest"), "initial") << "Error in getValueForParameter for unit_test.samsonSetupTest";


    EXPECT_EQ(samson::SamsonSetup::shared()->setValueForParameter("unit_test.samsonSetupTest", "successful"), true) << "Error in setValueForParameter for unit_test.samsonSetupTest";
    EXPECT_EQ(samson::SamsonSetup::shared()->getValueForParameter("unit_test.samsonSetupTest"), "successful") << "Error in getValueForParameter for unit_test.samsonSetupTest";

    EXPECT_EQ(samson::SamsonSetup::shared()->setValueForParameter("isolated.timeout", "1000"), true) << "Error in setValueForParameter for isolated.timeout";
    EXPECT_EQ(samson::SamsonSetup::shared()->getValueForParameter("isolated.timeout"), "1000") << "Error in getValueForParameter for isolated.timeout";

    //EXPECT_EQ(samson::SamsonSetup::shared()->getValueForParameter("non_existing_entry"), "1000") << "Error in getValueForParameter for non_existing_entry";

    samson::SamsonSetup::shared()->resetToDefaultValues();

    EXPECT_EQ(samson::SamsonSetup::shared()->isParameterDefined("unit_test.samsonSetupTest"), true) << "Error in isParameterDefined negative test";
    EXPECT_EQ(samson::SamsonSetup::shared()->getValueForParameter("unit_test.samsonSetupTest"), "initial") << "Error in getValueForParameter for unit_test.samsonSetupTest";

    EXPECT_EQ(samson::SamsonSetup::shared()->getValueForParameter("isolated.timeout"), "300") << "Error in getValueForParameter for isolated.timeout";

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

