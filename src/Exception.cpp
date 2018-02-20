//#include "fondoglobal.h"
#include <e32std.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "symbian_def.h"
#include "ansistring.h"
#include "portab_symbian.h"
#include "CenterCore.h"

void Qany_Exception (const char* _str) {
	TBuf8<64> _msg ((const TUint8*)_str);
	TBuf16<64> _new;
	
	FC_AsciiToUnicodeL (_new, _msg);
	CenterCore::DisplayMessage (_L("Qany Exception"), _new);
	FxAssert (0);
}

void Sxml_Exception (const char* _str) {

	CenterCore::DumpException( _str,0 );

	User::Leave( 0xff );
}

