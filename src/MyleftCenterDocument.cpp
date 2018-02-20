/*
============================================================================
 Name        : CMyleftCenterDocument from MyleftCenterDocument.h
 Author      : yilizheng
 Version     :
 Copyright   : MyleftCenter
 Description : CMyleftCenterDocument implementation
============================================================================
*/

// INCLUDE FILES
#include "MyleftCenterDocument.h"
#include "MyleftCenterAppui.h"
#include "CenterCore.h"
// ================= MEMBER FUNCTIONS =======================

// constructor
CMyleftCenterDocument::CMyleftCenterDocument(CEikApplication& aApp)
: CAknDocument(aApp)    
    {
    }

// destructor
CMyleftCenterDocument::~CMyleftCenterDocument()
{

	CenterCore* iplaycore=CenterCore::GetInstance();
	if(iplaycore)
		delete iplaycore;
#ifdef _S60_2RD_			
			Dll::FreeTls();			
#else
			centerExtern=NULL;
#endif
		
	FC_Logsp("~CMyleftCenterDocument");
	CloseSTDLIB();
}

// EPOC default constructor can leave.
void CMyleftCenterDocument::ConstructL()
    {
       	CenterCore* iplaycore=NULL;
#ifdef __WINS__	
    		iplaycore =CenterCore::NewL(_L("C:\\MyLeft\\"));
#else
		if(FC_IsDirectoryExist("E:\\"))
		{
			 FC_Log("CMyleftCenterDocument  DirectoryExist");
			iplaycore =CenterCore::NewL(_L("E:\\MyLeft\\"));
		}
		else
		{
			 FC_Log("CMyleftCenterDocument  Directory  Not Exist");
			 iplaycore =CenterCore::NewL(_L("C:\\Data\\MyLeft\\"));
		}
#endif

#ifndef _S60_3RD_
		Dll::SetTls(iplaycore);
#else
		centerExtern=iplaycore;
#endif

    }

// Two-phased constructor.
CMyleftCenterDocument* CMyleftCenterDocument::NewL(
        CEikApplication& aApp)     // CMyleftCenterApp reference
    {
    CMyleftCenterDocument* self = new (ELeave) CMyleftCenterDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }
    
// ----------------------------------------------------
// CMyleftCenterDocument::CreateAppUiL()
// constructs CMyleftCenterAppUi
// ----------------------------------------------------
//
CEikAppUi* CMyleftCenterDocument::CreateAppUiL()
    {

	 CMyleftCenterAppUi *appUI= new (ELeave) CMyleftCenterAppUi;
	
   	CenterCore* iCenterCore=CenterCore::GetInstance();
	if(iCenterCore)
		iCenterCore->m_appUI=appUI;
	return appUI;
    }

  
