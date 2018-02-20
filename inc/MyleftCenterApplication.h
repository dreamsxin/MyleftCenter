/*
============================================================================
 Name        : MyleftCenterApplication.h
 Author      : yilizheng
 Version     :
 Copyright   : MyleftCenter
 Description : Main application class
============================================================================
*/

#ifndef __MYLEFTCENTERAPPLICATION_H__
#define __MYLEFTCENTERAPPLICATION_H__

// INCLUDES
#include <aknapp.h>

// CLASS DECLARATION

/**
* CMyleftCenterApplication application class.
* Provides factory to create concrete document object.
* An instance of CMyleftCenterApplication is the application part of the
* AVKON application framework for the MyleftCenter example application.
*/
class CMyleftCenterApplication : public CAknApplication
    {
    public: // Functions from base classes

        /**
        * From CApaApplication, AppDllUid.
        * @return Application's UID (KUidMyleftCenterApp).
        */
        TUid AppDllUid() const;

    protected: // Functions from base classes

        /**
        * From CApaApplication, CreateDocumentL.
        * Creates CMyleftCenterDocument document object. The returned
        * pointer in not owned by the CMyleftCenterApplication object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
    };

#endif // __MYLEFTCENTERAPPLICATION_H__

// End of File
