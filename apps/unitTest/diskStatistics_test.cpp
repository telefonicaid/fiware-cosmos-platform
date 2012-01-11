/* ****************************************************************************
*
* FILE            diskOperationTest.cpp
*
* AUTHOR          Javier Lois
*
* DATE            December 2011
*
* DESCRIPTION
*
* unit testing of the diskStatistics and diskStatisticsItem classes in the engine library
*
*/

#include "gtest/gtest.h"

#include "engine/DiskStatistics.h"

#include "xmlparser/xmlParser.h"

//Test getInfo()
TEST(diskStatisticsTest, emptyTest) {
    engine::DiskStatistics stats;
    
    std::ostringstream info;
    stats.getInfo( info );
//std::cout<<info.str()<<std::endl;
    
    //read and check xml
    XMLNode xMainNode=XMLNode::parseString(info.str().c_str(),"");
    XMLNode readNode = xMainNode.getChildNode("read");
    EXPECT_EQ(std::string(readNode.getChildNode("description").getText()),
        "[ Currently    0 hits/s    0 B/s ] [ Last sample  00:00:00 ] [ Accumulated in  00:00:00    0 hits with    0 B ]") 
        << "Error writing read description tag";
    EXPECT_EQ(std::string(readNode.getChildNode("rate").getClear().lpszValue), "0") << "Error writing read rate tag";
    XMLNode writeNode = xMainNode.getChildNode("write");
    EXPECT_EQ(std::string(writeNode.getChildNode("description").getText()),
        "[ Currently    0 hits/s    0 B/s ] [ Last sample  00:00:00 ] [ Accumulated in  00:00:00    0 hits with    0 B ]") 
        << "Error writing write description tag";
    EXPECT_EQ(std::string(writeNode.getChildNode("rate").getClear().lpszValue), "0") << "Error writing write rate tag";
    XMLNode totalNode = xMainNode.getChildNode("total");
    EXPECT_EQ(std::string(totalNode.getChildNode("description").getText()),
        "[ Currently    0 hits/s    0 B/s ] [ Last sample  00:00:00 ] [ Accumulated in  00:00:00    0 hits with    0 B ]") 
        << "Error writing read description tag";
    EXPECT_EQ(std::string(totalNode.getChildNode("rate").getClear().lpszValue), "0") << "Error writing write rate tag";

}

//Test add()
TEST(diskStatisticsTest, addTest) {
    engine::DiskStatistics stats;
    
    //Add data
    stats.add(engine::DiskOperation::read, 3);
    stats.add(engine::DiskOperation::write, 4);
    stats.add(engine::DiskOperation::append, 5);

    //Javi: The result is time dependent. If I set this sleep here the result is fixed, 
    //but if I don't the result is very variable and cannot be tested. 
    //It makes the test execution time 1 second long which is a lot. Is there a better way? 
    //(using usleep with times smaller than a second gives undefined results too)
    //sleep(1);
    
    std::ostringstream info;
    stats.getInfo( info );
    //std::cout << info.str() << std::endl;
    
    //read and check xml
    XMLNode xMainNode=XMLNode::parseString(info.str().c_str(),"");
    XMLNode readNode = xMainNode.getChildNode("read");
    EXPECT_EQ(std::string(readNode.getChildNode("rate").getClear().lpszValue), "2") << "Error writing read rate tag";
    XMLNode writeNode = xMainNode.getChildNode("write");
    EXPECT_EQ(std::string(writeNode.getChildNode("description").getText()),
        "[ Currently    0 hits/s    0 B/s ] [ Last sample  00:00:00 ] [ Accumulated in  00:00:00    0 hits with    0 B ]") 
        << "Error writing write description tag";
    EXPECT_EQ(std::string(writeNode.getChildNode("rate").getClear().lpszValue), "8") << "Error writing write rate tag";
    XMLNode totalNode = xMainNode.getChildNode("total");
    EXPECT_EQ(std::string(totalNode.getChildNode("description").getText()),
        "[ Currently    0 hits/s    0 B/s ] [ Last sample  00:00:00 ] [ Accumulated in  00:00:00    0 hits with    0 B ]") 
        << "Error writing read description tag";
    EXPECT_EQ(std::string(totalNode.getChildNode("rate").getClear().lpszValue), "10") << "Error writing write rate tag";
    
}
    

