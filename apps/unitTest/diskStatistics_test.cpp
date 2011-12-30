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

#include "xmlmarkup/xmlmarkup.h"

//Test getInfo()
TEST(diskStatisticsTest, emptyTest) {
    engine::DiskStatistics stats;
    
    std::ostringstream info;
    stats.getInfo( info );
    
    //read and check xml
    CMarkup xmlData( info.str() );
    xmlData.FindElem("read");
    xmlData.IntoElem();
    xmlData.FindElem("description");
    EXPECT_EQ(xmlData.GetData(),
        "[ Currently    0 hits/s    0 B/s ] [ Last sample  00:00:00 ] [ Accumulated in  00:00:00    0 hits with    0 B ]") 
        << "Error writing read description tag";
    xmlData.FindElem("rate");
    EXPECT_EQ(xmlData.GetData(), "0") << "Error writing read rate tag";
    xmlData.OutOfElem();
    xmlData.FindElem("write");
    xmlData.IntoElem();
   xmlData.FindElem("description");
    EXPECT_EQ(xmlData.GetData(),
        "[ Currently    0 hits/s    0 B/s ] [ Last sample  00:00:00 ] [ Accumulated in  00:00:00    0 hits with    0 B ]") 
        << "Error writing write description tag";
     xmlData.FindElem("rate");
    EXPECT_EQ(xmlData.GetData(), "0") << "Error writing read rate tag";
    xmlData.OutOfElem();
    xmlData.FindElem("total");
    xmlData.IntoElem();
    xmlData.FindElem("description");
    EXPECT_EQ(xmlData.GetData(),
        "[ Currently    0 hits/s    0 B/s ] [ Last sample  00:00:00 ] [ Accumulated in  00:00:00    0 hits with    0 B ]") 
        << "Error writing read description tag";
    xmlData.FindElem("rate");
    EXPECT_EQ(xmlData.GetData(), "0") << "Error writing total rate tag";

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
    sleep(1);
    
    std::ostringstream info;
    stats.getInfo( info );
    //std::cout << info.str() << std::endl;
    
    //read and check xml
    CMarkup xmlData( info.str() );
    xmlData.FindElem("read");
    xmlData.IntoElem();
    xmlData.FindElem("rate");
    EXPECT_EQ(xmlData.GetData(), "2") << "Error writing read rate tag";
    xmlData.OutOfElem();
    xmlData.FindElem("write");
    xmlData.IntoElem();
    xmlData.FindElem("rate");
    EXPECT_EQ(xmlData.GetData(), "8") << "Error writing read rate tag";
    xmlData.OutOfElem();
    xmlData.FindElem("total");
    xmlData.IntoElem();
    xmlData.FindElem("rate");
    EXPECT_EQ(xmlData.GetData(), "10") << "Error writing read rate tag";
    xmlData.OutOfElem();
}
    

