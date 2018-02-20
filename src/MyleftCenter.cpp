/*
============================================================================
 Name        : MyleftCenter.cpp
 Author      : yilizheng
 Version     :
 Copyright   : MyleftCenter
 Description : Application entry point
============================================================================
*/

// INCLUDE FILES
#include <eikstart.h>
#include "MyleftCenterApplication.h"


LOCAL_C CApaApplication* NewApplication()
	{
	return new CMyleftCenterApplication;
	}

GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication( NewApplication );
	}
