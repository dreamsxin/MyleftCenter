#include "LrcSubtitle.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <UTF.H>
#include "CenterCore.h"
#include "portab_symbian.h"

_LIT( KLrcSubtitleExt, ".lrc" );

CLrcSubtitle::CLrcSubtitle(const char* strFile)
			:CSubtitle(strFile)
{
	return; 
}

CLrcSubtitle::~CLrcSubtitle()
{
}

TBool CLrcSubtitle::Load()
{
	CheckEncodeType();
	
//	if(GetEncode()==CSubtitle::EEncode_Unicode)
//		return	UnicodeLoad();

	FILE*	fp = fopen( iFile, "r");
	if( fp == NULL )
		return EFalse;

	char	szBuf[1024];
	memset(szBuf,0,1024);
	int k, l=0;
	CSubtitle::TSubtitleEncode   encode=GetEncode();
	while( fgets( szBuf, 1024, fp))
	{
		if( l==0 )
		{// first line
			l++;
			if( szBuf[0]=='\xEF' && szBuf[1]=='\xBB' && szBuf[2]=='\xBF' )
				SetEncode( CSubtitle::EEncode_Utf8 );
			if( szBuf[0]=='\xFF' && szBuf[1]=='\xFE' )
				SetEncode( CSubtitle::EEncode_Unicode );			
		}

		if(encode==CSubtitle::EEncode_Unicode)
		{
			TBuf8<512>   ansi;
			TPtrC16 	pUnicode;
			char *pBuf=(char*)ansi.PtrZ();

			
			if(szBuf[0]=='\x00')
			{
				char	tempBuf[1024];
				memcpy(tempBuf,szBuf+1,1023);
				memcpy(szBuf,tempBuf,1024);
			}

			int istrlen=CheckUnicodeStrlen(szBuf,1024);
			pUnicode.Set( (const TUint16*)szBuf ,istrlen);
			CnvUtfConverter::ConvertFromUnicodeToUtf8(ansi,pUnicode);
		//	FC_UnicodeToAsciiL(ansi, pUnicode);
			
			strcpy(szBuf,pBuf);
			
			SetEncode( CSubtitle::EEncode_Utf8);
		}

		
		char*	pTitle = strrchr( szBuf, ']');
		if( pTitle == NULL )
			continue;

		pTitle++;
		for( int i = strlen( pTitle) - 1 ; i > 0 ; i-- )
		{
			if( !isspace( *(pTitle+i)))
				break;

			*(pTitle + i) = '\0';
		}
		//k = strlen(pTitle);
		//if( k>0 && pTitle[k-1]=='\x0D' )
		//	pTitle[k-1] = '\0';
		RemoveCrLfFromText( pTitle );

		int	nStart = 0;
		while(1)
		{
			char*	pTime = strchr( szBuf + nStart, '[');
			if( pTime == NULL )
				break;

			TUint	dwMinute, dwSecond, dwMilliSecond;
			if( sscanf( pTime, "[%02d:%02d.%02d]", &dwMinute, &dwSecond, &dwMilliSecond) != 3 )
			{
				dwMilliSecond = 0;
				if( sscanf( pTime, "[%02d:%02d]", &dwMinute, &dwSecond ) != 2 )
					break;
			}

			TUint	dwTime = (dwMinute * 60 + dwSecond) * 1000 + dwMilliSecond;
			Insert( dwTime, pTitle);

		//	FC_Log("CLrcSubtitle::Load() dwTime=%d ,pTitle=%s" ,(int)dwTime, (const char*)pTitle );
			
			nStart += 10;
		}

		memset(szBuf,0,1024);
	}

	fclose(fp);
	return ETrue;
}
TBool CLrcSubtitle::Exists( const TDesC& aFileName, TDes& aSubtitleFile ) 
{
	TParse parser;
	parser.Set( aFileName, NULL, NULL );

	TFileName fileName;
	fileName.Copy( parser.DriveAndPath() );
	fileName.Append( parser.Name() );
	fileName.Append( _L(".lrc") );
	
	TInt ret = CenterCore::IsFileExist( fileName );
	if (ret)
		aSubtitleFile.Copy( fileName );

	return (TBool)ret;
}

TBool CLrcSubtitle::Exists( const TDesC& aPath, TDes& aFilename, TDes& aSubtitleFile  ) 
{
	TFileName fileName;
	fileName.Copy(aPath );
	fileName.Append(aFilename);
	fileName.Append( _L(".lrc") );
	
	TInt ret = CenterCore::IsFileExist( fileName );
	if (ret)
		aSubtitleFile.Copy( fileName );

	return (TBool)ret;
}

const TDesC& CLrcSubtitle::Ext() 
{
	return KLrcSubtitleExt;
}

void CLrcSubtitle::RemoveCrLfFromText(char* strTitle)
{
	int i, n = strlen(strTitle);
	if( n>0 && strTitle[n-1]=='\x0D' )
		strTitle[n-1] = '\0';
	for( i=0; i<n; i++ )
	{
		if( strTitle[i]=='\x0D'||strTitle[i]=='\x0A' )
			strTitle[i] = ' ';
	}
}

void CLrcSubtitle::CheckEncodeType()
{
	FC_Logsp("CheckEncodeType() --before is-- %d",GetEncode());
	FILE*	fp = fopen( iFile, "r");
	if( fp == NULL )
		return ;

	TInt 	filsize;
	TInt count_bad_utf=0,count_good_utf=0 ;
	char current_byte,previous_byte;
	char *_read_buf = (char*)malloc(2*1024);
	memset(_read_buf,0,2*1024);
	char *p=_read_buf;
	
	fseek( fp, 0, SEEK_END);
	filsize = ftell(fp);
	fseek( fp, 0, SEEK_SET);
	
	if(filsize>1024*2)
		filsize=1024*2;

	fread( _read_buf, 1, filsize, (FILE*)fp);
	FC_Logsp("CheckEncodeType() --filsize is-- %d",filsize);	
//	FC_Logsp("CheckEncodeType() --_read_buf is-- \n%s\n",_read_buf);	
	if( _read_buf[0]=='\xEF' && _read_buf[1]=='\xBB' && _read_buf[2]=='\xBF' )
		SetEncode( CSubtitle::EEncode_Utf8 );
	if( _read_buf[0]=='\xFF' && _read_buf[1]=='\xFE' )
		SetEncode( CSubtitle::EEncode_Unicode );

	if(GetEncode()==CSubtitle::EEncode_Ansi)  //maybe is utf8 no BOM
	{
		TInt i=0;
		while(i<filsize-1)
		{
			previous_byte=*p++;
			current_byte=*p;
			if ((current_byte & 0xC0) == 0x80)
			{
				if ((previous_byte & 0xC0) == 0xC0 )
				{
					count_good_utf ++;
				} 
				else if( (previous_byte & 0x80) == 0x00)
				{
					count_bad_utf ++;
				}
			} 
			else if ((previous_byte & 0xC0) == 0xC0 )
			{
					count_bad_utf ++;
			 }
			i++;
		}
		FC_Logsp("CheckEncodeType() --count_good_utf is-- %d,count_bad_utf=%d",count_good_utf,count_bad_utf);
		if ( count_good_utf >= count_bad_utf)
		{
			SetEncode( CSubtitle::EEncode_Utf8 );
		}

}

	free(_read_buf);
	fclose(fp);
	FC_Logsp("CheckEncodeType() is %d",GetEncode());
	return ;

}

int CLrcSubtitle::CheckUnicodeStrlen(char * _szBuf,int _MaxLen)
{
	int i=0;
	char *p=_szBuf;
	for(i=0;i<_MaxLen;i++,p++)
	{
		if('\x00'==*p && '\x00'==*(p+1))
		{
			return i;
		}
	}

	return _MaxLen;
}


int CLrcSubtitle::GetLineBuf(int *_pos,char * _fileBuf,char * _LineBuf,int _MaxLen)
{
	if(*_pos>=strlen(_fileBuf))
		return 0;

	char *p=_fileBuf+*_pos;
	int i=0;
	for(i=0;i<_MaxLen-*_pos;i++,p++)
	{
		if('\x0d'==*p && '\x0a'==*(p+1))
		{
			strncpy(_LineBuf,_fileBuf+*_pos,i);
			*_pos+=i+2;
			return i;
		}
	}

	
	return 0;
}

int CLrcSubtitle::UnicodeLoad()
{
	RFs _fs;
	RFile _file;
	TInt 	filsize;
	TBuf<256>  filename;

	FC_CharpToTBuf16(filename, iFile);
 	_fs.Connect ();
	 _file.Open (_fs, filename, EFileShareReadersOnly);
	_file.Size(filsize);

	HBufC8* hbuf8=HBufC8::NewL(filsize);
	TPtr8 pbuf8=hbuf8->Des();
	HBufC* hbufUnicode=HBufC::NewL(filsize);
	TPtr pbufUnicode=hbufUnicode->Des();


	char *_read_buf = (char*)malloc(filsize+1);
	char *p=_read_buf;
	char szBuf[512];

	memset(szBuf,0,512);
	memset(_read_buf,0,filsize+1);

	if (KErrNone != _file.Read (2,pbuf8)) 
	{
		delete hbuf8;
		delete hbufUnicode;
		free(_read_buf);
		_file.Close ();
		_fs.Close ();
		FC_Logsp("FPlayerCore::UnicodeLoad() _file.Read   failed ");
		return 0;
	}		

	TPtr8  preadBuf8((TUint8 *)_read_buf,filsize+1);
	TPtr pBuf16(NULL,0,0);
	pBuf16.Set((TUint16 *)pbuf8.Ptr(),pbuf8.Length(),filsize);
	pbufUnicode.Copy(pBuf16);

	CnvUtfConverter::ConvertFromUnicodeToUtf8(preadBuf8,pbufUnicode);
//	FC_TBuf16ToCharp(_read_buf,pbufUnicode);
	
	delete hbufUnicode;
	delete  hbuf8;
	_file.Close ();
	_fs.Close ();

	int k, l=0;
	int pos=0;

	SetEncode( CSubtitle::EEncode_Utf8 );
	while( GetLineBuf(&pos, _read_buf,szBuf, 512))
	{
		
		char*	pTitle = strrchr( szBuf, ']');
		if( pTitle == NULL )
			continue;

		pTitle++;
		for( int i = strlen( pTitle) - 1 ; i > 0 ; i-- )
		{
			if( !isspace( *(pTitle+i)))
				break;

			*(pTitle + i) = '\0';
		}
		//k = strlen(pTitle);
		//if( k>0 && pTitle[k-1]=='\x0D' )
		//	pTitle[k-1] = '\0';
		RemoveCrLfFromText( pTitle );

		int	nStart = 0;
		while(1)
		{
			char*	pTime = strchr( szBuf + nStart, '[');
			if( pTime == NULL )
				break;

			TUint	dwMinute, dwSecond, dwMilliSecond;
			if( sscanf( pTime, "[%02d:%02d.%02d]", &dwMinute, &dwSecond, &dwMilliSecond) != 3 )
			{
				dwMilliSecond = 0;
				if( sscanf( pTime, "[%02d:%02d]", &dwMinute, &dwSecond ) != 2 )
					break;
			}

			TUint	dwTime = (dwMinute * 60 + dwSecond) * 1000 + dwMilliSecond;
			Insert( dwTime, pTitle);

		//	FC_Log("CLrcSubtitle::UnicodeLoad() dwTime=%d ,pTitle=%s" ,(int)dwTime, (const char*)pTitle );
			
			nStart += 10;
		}

		memset(szBuf,0,1024);
	}

	free(_read_buf);
	return ETrue;
}
