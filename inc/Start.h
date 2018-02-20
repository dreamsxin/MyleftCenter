#ifndef _START_H_
#define _START_H_

//#include "define.h"

#include <e32std.h>
#include <e32base.h>
#include <e32def.h>
#include <e32math.h>
#include <eikapp.h>
#include <eikdoc.h>
#include <coeccntx.h>
#include <aknnavide.h>
#include <akntabgrp.h>
#include <coecntrl.h>
#include <fbs.h>
#include <eikimage.h>
#include <eiklabel.h>
#include <akndoc.h>
#include <aknview.h>
#include <eikgted.h>
#include <f32file.h>
#include <hal.h>

#include <aknapp.h>
#include <coecobs.h>
#include <AknGlobalListQuery.h>
#include <AknQueryDialog.h>
#include <aknpopup.h>
#include <aknviewappui.h>
#include <akntitle.h>
#include <badesca.h>
#include <akniconarray.h>
#include <eikclbd.h>
#include <barsread.h>
#include <aknlists.h>

#include <cdbcols.h>
#include <commdb.h>
#include <apselect.h>

#include <es_sock.h>
#include <in_sock.h>

#include <Avkon.hrh>

#include <MyleftCenter.rsg>

#include "ansistring.h"
#include "sxml.h"
#include "MyleftCenter.hrh"
#include "CenterCore.h"
#include "qany.h"
#include "const.h"



typedef enum _TScreenResolution
{
	EScrn_Unknown,
	EScrn_176x208,
	EScrn_240x320,
	EScrn_320x240,
	EScrn_208x208
}TScreenResolution;

#endif
