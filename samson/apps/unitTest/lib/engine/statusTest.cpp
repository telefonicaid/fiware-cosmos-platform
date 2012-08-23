#include <fcntl.h>

#include "gtest/gtest.h"
#include "samson/common/Visualitzation.h"
#include "samson/common/status.h"

// Test  status;
TEST(commonTest, status) {
  EXPECT_EQ(strcmp(samson::status(samson::OK), "OK") == 0, true) << "Error in status OK";
  EXPECT_EQ(strcmp(samson::status(samson::NotImplemented),
                   "Not Implemented") == 0, true) << "Error in status NotImplemented";
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
  EXPECT_EQ(strcmp(samson::status(samson::GetHostByNameError),
                   "Get Host By Name Error") == 0, true) << "Error in status GetHostByNameError";
  EXPECT_EQ(strcmp(samson::status(samson::BindError), "Bind Error") == 0, true) << "Error in status BindError";
  EXPECT_EQ(strcmp(samson::status(samson::ListenError), "Listen Error") == 0, true) << "Error in status ListenError";
  EXPECT_EQ(strcmp(samson::status(samson::ReadError), "Read Error") == 0, true) << "Error in status ReadError";
  EXPECT_EQ(strcmp(samson::status(samson::WriteError), "Write Error") == 0, true) << "Error in status WriteError";
  EXPECT_EQ(strcmp(samson::status(samson::Timeout), "Timeout") == 0, true) << "Error in status Timeout";
  EXPECT_EQ(strcmp(samson::status(samson::ConnectionClosed),
                   "Connection Closed") == 0, true) << "Error in status ConnectionClosed";
  EXPECT_EQ(strcmp(samson::status(samson::PThreadError), "Thread Error") == 0, true) << "Error in status PThreadError";
  EXPECT_EQ(strcmp(samson::status(samson::WrongPacketHeader),
                   "Wrong Packet Header") == 0, true) << "Error in status WrongPacketHeader";
  EXPECT_EQ(strcmp(samson::status(samson::ErrorParsingGoogleProtocolBuffers),
                   "Error parsing Google Protocol Buffer message") == 0,
            true) << "Error in status ErrorParsingGoogleProtocolBuffers";

  EXPECT_EQ(samson::au_status(au::OK), samson::OK) << "Error in au_status OK";
  EXPECT_EQ(samson::au_status(au::Error), samson::Error) << "Error in au_status Error";
  EXPECT_EQ(samson::au_status(au::NotImplemented), samson::NotImplemented) << "Error in au_status NotImplemented";
  EXPECT_EQ(samson::au_status(au::GetHostByNameError),
            samson::GetHostByNameError) << "Error in au_status GetHostByNameError";
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
  EXPECT_EQ(samson::au_status(au::GPB_NotInitializedMessage),
            samson::Error) << "Error in au_status GPB_NotInitializedMessage";
  EXPECT_EQ(samson::au_status(au::GPB_WriteErrorSerializing),
            samson::Error) << "Error in au_status GPB_WriteErrorSerializing";
  EXPECT_EQ(samson::au_status(au::GPB_WriteError), samson::Error) << "Error in au_status GPB_WriteError";
}

// Test  Visualization;
TEST(commonTest, Visualization) {
  EXPECT_EQ(samson::match("*", "string"), true) << "Error in match wildcard";
  EXPECT_EQ(samson::match("[A-Z][A-Z]*", "TEST"), true) << "Error in match capital";
  EXPECT_EQ(samson::match("[A-Z][A-Z]*", "test"), false) << "Error in match small";
}
