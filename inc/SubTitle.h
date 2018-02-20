#ifndef SUBTITLE_H
#define SUBTITLE_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <CHARCONV.H>

class CSubtitle : public CBase
{

public:
	enum { SUB_STEP = 20 };
	
	enum TSubtitleEncode
	{
		EEncode_Ansi,
		EEncode_Utf8,
		EEncode_Unicode
	};
public:
	CSubtitle(const char* aFileName);	
	virtual ~CSubtitle();
	
	virtual TBool Load() = 0;
	void Seek(TUint dwTime);
	void SetOffset(TInt dwTime){offset+=dwTime;}
	int	GetTextBuf(TUint& dwTime, TUint16* pData, TUint& dwTextLen, TUint dwBufLen);
	void Insert(TUint dwTime, const char* szSubtitle);
	TUint GetMaxLength();

	TSubtitleEncode GetEncode();
	void SetEncode(TSubtitleEncode aEncode);

	TUint GetCurrentFrameIndex() const;
	int GetTextBufByIndex(TUint aFrameIndex, TUint16* pData, TUint& dwTextLen, TUint dwBufLen);
	TUint GetFrameCount() const;
protected:
	TInt CharConvert(TDes16& aUnicodeBuffer, const TDesC8& aBufferBytes);
	
protected:
	char iFile[KMaxFileName];
	TUint16**	iSubtitle;
	TUint	iMaxLen;
	TUint*	iTime;
	TUint	iSize;
	TUint	iCount;
	TInt		offset;
	TUint	iCurFrame;
	
protected:
	TSubtitleEncode iEncode;
	CCnvCharacterSetConverter* iCharConverter;
};


#endif //SUBTITLE_H