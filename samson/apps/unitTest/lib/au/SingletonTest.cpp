#include <fcntl.h>

#include "au/Singleton.h"
#include "gtest/gtest.h"
#include "unitTest/test_common.h"

class Example
{

public:

   Example()
   {
	  value_=0;
   }

   int value()
   {
	  return value_;
   }

   void set_value( int value )
   {
	  value_ = value;
   }

private:

   int value_;

};

TEST(au_Singleton, basic) {

  Example* example = au::Singleton<Example>::shared();
  example->set_value(1);

  Example* example2 = au::Singleton<Example>::shared();

  EXPECT_EQ( 1 , example2->value() );

}
