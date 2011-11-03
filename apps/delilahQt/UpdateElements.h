
#ifndef _H_DELILAH_UPDATE_ELEMENTS
#define _H_DELILAH_UPDATE_ELEMENTS

#include "au/Tree.h" // au::TreeItem

class DelilahQTElement;

// Function to interact with the common list of elements
void addDelilagQTElement( DelilahQTElement* );
void removeDelilagQTElement( DelilahQTElement* );
void updateDelilagQTElements( );


// TreeItem
au::TreeItem* mainTreeItem();
void replaceMainTreeItem( au::TreeItem* treeItem );

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
   virtual void update( au::TreeItem * item , bool complete_update ) = 0;


};

#endif
