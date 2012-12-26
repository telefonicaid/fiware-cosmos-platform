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

#ifndef _H_DELILAH_UPDATE_ELEMENTS
#define _H_DELILAH_UPDATE_ELEMENTS

#include "tables/Tree.h" // au::tables::TreeItem

class DelilahQTElement;

// Function to interact with the common list of elements
void addDelilagQTElement( DelilahQTElement* );
void removeDelilagQTElement( DelilahQTElement* );
void updateDelilagQTElements( );


// TreeItem
au::tables::TreeItem* mainTreeItem();
void replaceMainTreeItem( au::tables::TreeItem* treeItem );

class DelilahQTElement
{

public:

   DelilahQTElement()
   {
	  addDelilagQTElement( this );
   }

   ~DelilahQTElement()
   {
	  removeDelilagQTElement( this );
   }

   // Update with the top level tree item
   virtual void update( bool complete_update ) = 0;

};

#endif
