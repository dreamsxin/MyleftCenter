/*
============================================================================
 Name        : CMyleftCenterDocument from MyleftCenterDocument.h
 Author      : yilizheng
 Version     :
 Copyright   : MyleftCenter
 Description : Declares document for application.
============================================================================
*/

#ifndef MYLEFTCENTERDOCUMENT_H
#define MYLEFTCENTERDOCUMENT_H

// INCLUDES
#include <akndoc.h>
   
// CONSTANTS

// FORWARD DECLARATIONS
class  CEikAppUi;

// CLASS DECLARATION

/**
*  CMyleftCenterDocument application class.
*/
class CMyleftCenterDocument : public CAknDocument
    {
    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CMyleftCenterDocument* NewL(CEikApplication& aApp);

        /**
        * Destructor.
        */
        virtual ~CMyleftCenterDocument();

    public: // New functions

    public: // Functions from base classes
    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * EPOC default constructor.
        */
        CMyleftCenterDocument(CEikApplication& aApp);
        void ConstructL();

    private:

        /**
        * From CEikDocument, create CMyleftCenterAppUi "App UI" object.
        */
        CEikAppUi* CreateAppUiL();
    };

#endif



