#ifndef LRCSUBTITLE_H
#define LRCSUBTITLE_H

#include "SubTitle.h"

class CLrcSubtitle : public CSubtitle
{
public:
	

	CLrcSubtitle(const char* strFile);
	virtual ~CLrcSubtitle();

	virtual TBool Load();
	static TBool Exists( const TDesC& aPath, TDes& aFilename, TDes& aSubtitleFile  ) ;
	static TBool Exists(const TDesC& aFileName, TDes& aSubtitleFile);
	static const TDesC& Ext();

protected:
	void RemoveCrLfFromText(char* strTitle);
	void CheckEncodeType();
	int CheckUnicodeStrlen(char * _szBuf,int _MaxLen);
	int GetLineBuf(int *_pos,char * _fileBuf,char * _LineBuf,int _MaxLen);
	int UnicodeLoad();
};

#endif //LRCSUBTITLE_H
