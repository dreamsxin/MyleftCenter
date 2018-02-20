/*
============================================================================
 Name        : MyleftCenterApplication.cpp
 Author      : yilizheng
 Version     :
 Copyright   : MyleftCenter
 Description : Main application class
============================================================================
*/

// INCLUDE FILES
#include "MyleftCenterDocument.h"
#include "MyleftCenterApplication.h"

// ============================ MEMBER FUNCTIONS ===============================

// UID for the application;
// this should correspond to the uid defined in the mmp file
const TUid KUidMyleftCenterApp = { 0xe41e38a7 };

// -----------------------------------------------------------------------------
// CMyleftCenterApplication::CreateDocumentL()
// Creates CApaDocument object
// -----------------------------------------------------------------------------
//
CApaDocument* CMyleftCenterApplication::CreateDocumentL()
    {
    // Create an MyleftCenter document, and return a pointer to it
    return (static_cast<CApaDocument*>
                    ( CMyleftCenterDocument::NewL( *this ) ) );
    }

// -----------------------------------------------------------------------------
// CMyleftCenterApplication::AppDllUid()
// Returns application UID
// -----------------------------------------------------------------------------
//
TUid CMyleftCenterApplication::AppDllUid() const
    {
    // Return the UID for the MyleftCenter application
    return KUidMyleftCenterApp;
    }

// End of File
