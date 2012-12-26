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
#include "au/containers/GeneralDictionary.h"
#include "au/containers/SharedPointer.h"
#include "gtest/gtest.h"

class A
{
};
class B
{
};

TEST(au_containers_GlobalDictionary, assignation) {

   au::SharedPointer<A> a( new A() );
   au::SharedPointer<B> b( new B() );

  au::GeneralDictionary dictionary;

  dictionary.Set("a", a );
  dictionary.Set("b", b );

  au::SharedPointer<A> aa = dictionary.Get<A>("a");
  au::SharedPointer<A> ab = dictionary.Get<A>("b");
  au::SharedPointer<B> ba = dictionary.Get<B>("a");
  au::SharedPointer<B> bb = dictionary.Get<B>("b");


  EXPECT_TRUE(aa!=NULL);
  EXPECT_TRUE(ab==NULL);
  EXPECT_TRUE(ba==NULL);
  EXPECT_TRUE(bb!=NULL);
}

