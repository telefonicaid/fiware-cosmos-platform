
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
