#include "Subtitle.h"
#include <string.h>
#include <stdlib.h>
#include <eikenv.h>
#include "portab_symbian.h"

CSubtitle::CSubtitle(const char* aFileName)
{
	strcpy(iFile, aFileName);
	iMaxLen = 0;
	iSize = 0;
	iCount = 0;
	iTime = NULL;
	iSubtitle = NULL;

	iCurFrame = 0;
	iEncode = EEncode_Ansi;
}

CSubtitle::~CSubtitle()
{
	if( iTime )
	{
		free(iTime);
		iTime = NULL;
	}

	if( iSubtitle )
	{
		for( int i = 0 ; i < iCount ; i++ )
		{
			if( iSubtitle[i] )
				free(iSubtitle[i]);
		}
		free(iSubtitle);
		iSubtitle = NULL;
	}

	iMaxLen = 0;
	iCount = 0;
	iSize = 0;

	if(iCharConverter)
		delete iCharConverter;
}

void CSubtitle::Seek(TUint dwTime)
{
	int i;
	TUint dwSeekTime=dwTime;
	if(offset>0)
	{
		dwSeekTime+=offset;
	}
	else if(offset<0)
	{
		if(dwSeekTime>(-offset))
			dwSeekTime+=offset;		
	}
	
	for( i = 0 ; i < iCount ; i++ )
	{
	//	FC_Logsp("dwTime=%d,iTime[%d] =%d",(int)dwTime,i,iTime[i]);
		if( iTime[i] > dwSeekTime )
			break;
	}

	if( i > 0 )
		iCurFrame = i - 1;
	else
		iCurFrame = i;
}

int	CSubtitle::GetTextBuf(TUint& dwTime, TUint16* pData, TUint& dwTextLen, TUint dwBufLen)
{
	if( iCurFrame >= iCount )
		return 0;

	dwTime = iTime[iCurFrame];

	memset( pData, 0, dwBufLen*sizeof(TUint16) );
	TUint dwSrcLen = wcslen( (const wchar_t*)iSubtitle[iCurFrame]);

	if( dwSrcLen >= dwBufLen-1 )
	{
		memcpy( pData, (TUint16*)iSubtitle[iCurFrame], (dwBufLen-1)*sizeof(TUint16));
		dwTextLen = dwBufLen-1;
	}
	else
	{
		memcpy( pData, (TUint8*)iSubtitle[iCurFrame], dwSrcLen*sizeof(TUint16));
		dwTextLen = dwSrcLen;
	}
	iCurFrame++;
	return 1;
}

void CSubtitle::Insert(TUint dwTime, const char* szSubtitle)
{
	iCount++;
	if( iCount > iSize )
	{
		iSize += SUB_STEP;
		iSubtitle = (TUint16**)realloc( iSubtitle, sizeof(TUint16*) * iSize);
		iTime = (TUint*)realloc( iTime, sizeof(TUint) * iSize);
	}

	TUint	i = 0;
	for( i = 0 ; i < iCount - 1 ; i++ )
	{
		if( iTime[i] > dwTime )
			break;
	}

	for( TUint j = iCount - 1 ; j > i ; j-- )
	{
		iTime[j] = iTime[j-1];
		iSubtitle[j] = iSubtitle[j-1];
	}
	iTime[i] = dwTime;
	
	TUint len = strlen(szSubtitle);
	iSubtitle[i] = (TUint16*) malloc( (len+1)*sizeof(TUint16) );
	memset(iSubtitle[i],0,(len+1)*sizeof(TUint16));
	TPtr16 unicodePtr(iSubtitle[i], len+1);
	CharConvert( unicodePtr, TPtrC8((const TUint8*)szSubtitle) );
	

	TUint	dwLen = len;
	if( dwLen > iMaxLen )
		iMaxLen = dwLen;
}

TUint CSubtitle::GetMaxLength()
{
	return iMaxLen;
}

CSubtitle::TSubtitleEncode CSubtitle::GetEncode() 
{
	return iEncode;
}

void CSubtitle::SetEncode( CSubtitle::TSubtitleEncode aEncode ) 
{
	iEncode = aEncode;
}

TUint CSubtitle::GetCurrentFrameIndex() const
{
	return iCurFrame;
}

int CSubtitle::GetTextBufByIndex(TUint aFrameIndex, TUint16* pData, TUint& dwTextLen, TUint dwBufLen)
{
	if( aFrameIndex >= iCount )
		return 0;

	memset( pData, 0, dwBufLen*sizeof(TUint16));
	TUint dwSrcLen = wcslen( (const wchar_t*)iSubtitle[aFrameIndex]);

	if( dwSrcLen >= dwBufLen-1 )
	{
		memcpy( pData, (TUint16*)iSubtitle[aFrameIndex], (dwBufLen-1)*sizeof(TUint16));
		dwTextLen = dwBufLen-1;
	}
	else
	{
		memcpy( pData, (TUint16*)iSubtitle[aFrameIndex], dwSrcLen*sizeof(TUint16));
		dwTextLen = dwSrcLen;
	}
	
	return 1;
}

TUint CSubtitle::GetFrameCount() const
{
	return iCount;
}

TInt CSubtitle::CharConvert(TDes16& aUnicodeBuffer, const TDesC8& aBufferBytes)
{
	if( !iCharConverter )	
	{
		iCharConverter = CCnvCharacterSetConverter::NewL ();
		if( GetEncode() == CSubtitle::EEncode_Utf8 )
		{
			/* check if support UTF-8 */
			if (CCnvCharacterSetConverter::EAvailable !=
				iCharConverter->PrepareToConvertToOrFromL (
				KCharacterSetIdentifierUtf8, CEikonEnv::Static()->FsSession())) 
			{
				User::Leave (KErrNotSupported);
			}
		}
		else
		{
			/* check if support Gbk */
			if (CCnvCharacterSetConverter::EAvailable !=
				iCharConverter->PrepareToConvertToOrFromL (
				KCharacterSetIdentifierGbk, CEikonEnv::Static()->FsSession())) 
			{
				/* check if support GB2312 */
				if (CCnvCharacterSetConverter::EAvailable !=
					iCharConverter->PrepareToConvertToOrFromL (
					KCharacterSetIdentifierGb2312, CEikonEnv::Static()->FsSession())) 
				{
					/* not a chinese device, so use ASCII */
					if (CCnvCharacterSetConverter::EAvailable !=
						iCharConverter->PrepareToConvertToOrFromL (
						KCharacterSetIdentifierAscii, CEikonEnv::Static()->FsSession())) 
					{
						/* not support ASCII too */
						User::Leave (KErrNotSupported);
					}
				}
			}
		}
	}
	
	TInt _state = CCnvCharacterSetConverter::KStateDefault;
	TInt _ret = iCharConverter->ConvertToUnicode (
						aUnicodeBuffer, aBufferBytes, _state);
						
	return _ret;
}
