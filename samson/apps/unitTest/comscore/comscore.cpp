
#include <stdlib.h>

#include "gtest/gtest.h"
#include "comscore/SamsonComscoreDictionary.h"


//Test ReadFile( std::string _fileName );
TEST( comscore , test1 ) 
{
    samson::comscore::SamsonComscoreDictionary samson_comscore_dictionary;
    samson_comscore_dictionary.read( "test/comscore/samson_comscore_dictionary.bin" );

    const char* url = "a.ES.ZONAURAL.wikia.com";
    
    
    std::vector<uint> categories = samson_comscore_dictionary.getCategories( url );
    
    //printf("Categoried %lu\n" , categories.size() );
    
    ASSERT_TRUE( categories.size() == 2 );
    
    const char* cat1 = samson_comscore_dictionary.getCategoryName( categories[0] );
    const char* cat2 = samson_comscore_dictionary.getCategoryName( categories[1] );
    
    //printf("%s , %s" , cat1 , cat2 );
    
    EXPECT_TRUE( strcmp( cat1, "Community" ) == 0 );
    EXPECT_TRUE( strcmp( cat2, "Entertainment" ) == 0 );
    
}
