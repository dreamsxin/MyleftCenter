/*
============================================================================
 Name        : CMyleftCenterApp from MyleftCenterApp.h
 Author      : yilizheng
 Version     :
 Copyright   : MyleftCenter
 Description : Declares main application class.
============================================================================
*/

#ifndef MYLEFTCENTERAPP_H
#define MYLEFTCENTERAPP_H

// INCLUDES
#include <aknapp.h>

// CONSTANTS
// UID of the application
const TUid KUidMyleftCenter = { 0xe41e38a7 };

// CLASS DECLARATION

/**
* CMyleftCenterApp application class.
* Provides factory to create concrete document object.
* 
*/
class CMyleftCenterApp : public CAknApplication
    {
    
    public: // Functions from base classes
    private:

        /**
        * From CApaApplication, creates CMyleftCenterDocument document object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
        
        /**
        * From CApaApplication, returns application's UID (KUidMyleftCenter).
        * @return The value of KUidMyleftCenter.
        */
        TUid AppDllUid() const;
    };

#endif



