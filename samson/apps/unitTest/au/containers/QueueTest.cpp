#include "gtest/gtest.h"
#include "au/containers/SharedPointer.h"
#include "au/containers/Queue.h"
#include "unitTest/test_common.h"

TEST(au_containers_Queue , assignation )
{
   EXPECT_EQ( TestBase::get_num_instances() , 0 ) << "Error in number of instances of test class";

   {
	  au::Queue<TestBase> queue;
	  
	  queue.Push( new TestBase(10) );
	  queue.Push( new TestBase(11) );
	  queue.Push( new TestBase(12) );
	  queue.Push( new TestBase(13) );
	  // Make sure the counter works

	  EXPECT_EQ( TestBase::get_num_instances() > 0 , true ) << "Something wrong with the instance counter for test class";

	  
	  EXPECT_EQ( queue.Pop()->v() , 10 ) << "Error in au::Queue basic operations";
	  EXPECT_EQ( queue.Pop()->v() , 11 ) << "Error in au::Queue basic operations";
	  EXPECT_EQ( queue.Pop()->v() , 12 ) << "Error in au::Queue basic operations";
	  EXPECT_EQ( queue.Pop()->v() , 13 ) << "Error in au::Queue basic operations";
   }

   EXPECT_EQ( TestBase::get_num_instances() , 0 ) << "Error in number of instances of test class";

}

