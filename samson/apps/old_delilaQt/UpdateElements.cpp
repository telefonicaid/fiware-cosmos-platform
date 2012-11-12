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

#include "au/Token.h"
#include "au/TokenTaker.h"
#include "au/Cronometer.h"

#include "UpdateElements.h"  // Own interface

// List of elements to be updated os screen
std::set<DelilahQTElement*> updateElements;


au::tables::TreeItem* _mainTreeItem = NULL;
au::Token token_mainTreeItem("_mainTreeItem");

au::tables::TreeItem* mainTreeItem()
{
   au::TokenTaker tt( &token_mainTreeItem );
   if( !_mainTreeItem )
	  return NULL;

   au::tables::TreeItem* tmp = _mainTreeItem;
   _mainTreeItem = NULL;
   return tmp;
}

void replaceMainTreeItem( au::tables::TreeItem* treeItem )
{
   au::TokenTaker tt( &token_mainTreeItem );

   if( _mainTreeItem )
	  delete _mainTreeItem;
   _mainTreeItem = treeItem;
}

// Function to interact with the common list of elements
void addDelilagQTElement( DelilahQTElement* element )
{
   updateElements.insert( element );
}

void removeDelilagQTElement( DelilahQTElement* element )
{
   updateElements.erase( element );
}

au::Cronometer cronometer; // Cronometer to make sure we do not refresh too much

void updateDelilagQTElements(  )
{
   au::tables::TreeItem* item =  mainTreeItem();
   if( !item )
	  return; // Not possible to update


   bool complete_update = false;
   if( cronometer.diffTimeInSeconds() > (int) updateElements.size() )
   {
	  cronometer.reset();
	  complete_update= true;
   }

   // Update calls
   std::set<DelilahQTElement*>::iterator it_updateElements;
   for( it_updateElements = updateElements.begin() ; it_updateElements != updateElements.end() ; it_updateElements++ )
	  (*it_updateElements)->update( item , complete_update );	  




}

