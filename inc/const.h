#ifndef _FONDO_CONST_H_
#define _FONDO_CONST_H_

#include <e32std.h>
#include <e32base.h>
#include <e32def.h>
#include "symbian_def.h"

_LIT (KMultiBitmapFilename, "\\resource\\apps\\Fondoplayer3_aif.mif");
_LIT (KMultiIconFilename, "\\resource\\apps\\Fondoplayer3_aif.mif");
_LIT (ACTIVATE_COVER_PATH, "C:\\System\\Data\\FondoPlayer3\\Cover\\");
_LIT (INSTALL_FILENAME, "fp.sis");

_LIT (KPLAYLIST_FILENAME, "C:\\System\\Data\\FondoPlayer3\\PlayList");
_LIT (KBOOKMARK_FILENAME, "C:\\System\\Data\\FondoPlayer3\\Bookmark.xml");  
_LIT (KDOWNLOADS_FILENAME, "C:\\System\\Data\\FondoPlayer3\\Downloads.xml");
_LIT (KDOWNLOADOK_FILENAME, "C:\\System\\Data\\FondoPlayer3\\Downloadok.xml");
_LIT (START_SCRIPT_FILENAME, "C:\\System\\Data\\FondoPlayer3\\StartScript");
//_LIT (IMAGE_SAVE_PATH, "C:\\Data\\Images\\");
_LIT (KPLAYSTRATEGY_FILENAME, "C:\\System\\Data\\FondoPlayer3\\Protected.db");
_LIT (KPLAYSTRATEGY_FILENAME2, "C:\\System\\PlayStrategy%S.db");
_LIT (KFID_FILENAME, "C:\\System\\Data\\FondoPlayer3\\fondoid.db");
_LIT (KFID_FILENAME2, "C:\\System\\fondofid%S.db");
_LIT (KPLUGIN_FILEPATH, "C:\\System\\Data\\FondoPlayer3\\");
_LIT (KINTRO_FILENAME, "C:\\System\\Data\\FondoPlayer3\\intro.1");
_LIT (KBILL_SAVE_PATH, "C:\\System\\Data\\FondoPlayer3\\saved\\");
_LIT (KBILL_SAVE_PATH2, "C:\\System\\");
_LIT (KHELP_FILENAME, "file://c:/private/AF00018A/help.wml");
_LIT (KPLAYTASK_FILENAME, "C:\\System\\Data\\FondoPlayer3\\playtask.fp1");
_LIT (KSRCHLIST_FILENAME, "C:\\System\\Data\\FondoPlayer3\\searchlist.db");
_LIT (KHOTLIST_FILENAME, "C:\\System\\Data\\FondoPlayer3\\Hotlist.xml");  
_LIT (KVERSION_FILENAME, "C:\\System\\Data\\FondoPlayer3\\Version.db");
_LIT (KLOCALAD_FILENAME, "C:\\System\\Data\\FondoPlayer3\\LocalAd.db");
_LIT (KLOCALAD_FILENAME2, "C:\\System\\fondoad%S.db");
_LIT (KDAEMON_FILEPATH, "C:\\System\\Data\\FondoPlayer3\\daemon\\");
_LIT (KMESSAGE_FILEPATH, "C:\\System\\Data\\FondoPlayer3\\message\\");
_LIT (KMESSAGE_READ_FILEPATH, "C:\\System\\Data\\FondoPlayer3\\message\\read\\");
_LIT (KRING_SAVE_PATH, "C:\\Nokia\\Sounds\\Digital\\");
_LIT (KABOUT_FILENAME, "C:\\System\\Data\\FondoPlayer3\\about.txt");
_LIT (KRSSHELP_FILENAME, "C:\\System\\Data\\Fondoplayer3\\rss.txt");
_LIT (KEXCEPTION_FILENAME, "C:\\System\\Data\\Fondoplayer3\\Exception.txt");
_LIT (KSINASPCONFIG_FILENAME_TEMP, "C:\\System\\Data\\SinaMusic\\spconfig.db2");

_LIT (KSmallFontName, "CombinedChinesePlain12");
_LIT (KBigFontName, "CombinedChinesePlain16");

_LIT (KSONG_SAVE_PATH, "\\MyleftCenter\\Sounds\\");
/* App */
_LIT (PAYFSX_1YUAN_FILENAME, "C:\\System\\Data\\FondoMusic\\PaySignle.fsx");
_LIT (PAYFSX_2YUAN_FILENAME, "C:\\System\\Data\\FondoMusic\\PayDouble.fsx");

/* Main Script */
_LIT (KMSCRIPT_FILENAME, "C:\\System\\Data\\FondoPlayer3\\MScript");
_LIT (KAppPlayList, "PLST");
_LIT (KAppFile, "File");
_LIT (KAppRss, "RSS");
_LIT (KAppDownload, "DOWN");
_LIT (KAppBkmk, "BKMK");
_LIT (KAppPush, "PUSH");
_LIT (KAppUrl, "URL");
_LIT (KAppOpt, "OPT");
_LIT (KAppHelp, "HELP");
_LIT (KAppNetSearch, "SRCH");
_LIT (KAppIntroduce, "INTR");

_LIT (KSinaVision, "0");

#define KMainScriptArrayGranularity		32
#define KMaxAppControlNumber			16
#define KAppControlTextLength			32

/* Play list */
#define KPlstArrayGranularity			4
#define KMaxPlstDisplayNameLength		40
#define KMaxPlstFileLength				256 * 1024
#define KSelectFilesArrayGranularity	128
#define KMaxRecentItemCount				10

/* Bookmark */
#define KMaxBookmarkDisplayNameLength	40
#define KMaxBookmarkFileLength			32 * 1024

_LIT (KMyDocTag, "{MYDOC}");
_LIT (KFunExt, ".FUN");
_LIT (K3gpExt, ".3GP");
_LIT (KMp3Ext, ".MP3");
_LIT (KMp4Ext, ".MP4");
_LIT (KMovExt, ".MOV");
_LIT (KM4aExt, ".M4A");
_LIT (KFptExt, ".fpt");	// protected type
_LIT (KFadExt, ".fad");	// local ad file type
_LIT (KMsgExt, ".msg");

_LIT (KFsxExt, ".fsx");
_LIT (KTempFsxName, "tempfsx");
_LIT (KExchangeFileName, "exchange.dat");
_LIT (KDownloadFileExt, ".FP!");
_LIT (KTempUpgradeFileName, "fupgrade");
_LIT (KBackImgFileName, "backimg.mbm");
_LIT (KFunPosInfoFileName, "funposinfo.1");
_LIT8 (KMIMEFsxType, "video/fsx");

_LIT (KFileSource, "file");
_LIT (KRtspSource, "rtsp");
_LIT (KHttpSource, "http");

_LIT (KRtspProtocol, "rtsp");
_LIT (KRtspfProtocol, "rtspf");
_LIT (KHttpProtocol, "http");
_LIT (KFsxProtocol, "fsx");

_LIT (KTempHttpGetFileName, "myleft_temphttp.dat");

#define KBackUnknownView		0
#define KBackHomeView			1
#define KBackFavoriteView		2
#define KBackMediaPortalView	3
#define KBackPlaylistView		4
#define One_Page_Lenth		8
//in use
#ifdef _S60_3RD_
#define KFirstFlag	"C:\\System\\Data\\MyleftCenter\\MyleftCenter.flg"
#define KUpdataListFlag	"C:\\System\\Data\\MyleftCenter\\MyleftUpdataList.flg"
_LIT(KReadAboutFilename	,"C:\\System\\Data\\MyleftCenter\\ReadAbout.txt");
_LIT (KBackPictrueFolderC, "C:\\System\\Data\\MyleftCenter\\BackPic\\");	
_LIT (KBackPictrueFolderE, "E:\\MyLeft\\BackPic\\");
_LIT (CONFIG_FILENAME, "C:\\System\\Data\\MyleftCenter\\Config.xml");
_LIT (READLOCAL_FILENAME, "C:\\System\\Data\\MyleftCenter\\ReadLocalList.xml");
_LIT (ReadMask_FileName, "C:\\System\\Data\\MyleftCenter\\ReadMaskList.xml");
_LIT (MusicLocal_FileName, "C:\\System\\Data\\MyleftCenter\\MusicLocalList.xml");
_LIT (MusicMask_FileName, "C:\\System\\Data\\MyleftCenter\\MusicMaskList.xml");
_LIT (DownLoad_FileName, "C:\\System\\Data\\MyleftCenter\\DownLoadList.xml");
_LIT (DEFAULT_CONFIG_FILENAME, "C:\\System\\Data\\MyleftCenter\\DefConfig.xml");
_LIT (OEM_FILENAME, "C:\\System\\Data\\MyleftCenter\\oem.xml");
_LIT (KMusicMultiBitmapFilename, "\\resource\\apps\\MyleftCenter.mbm");	
#else
#define KFirstFlag	"C:\\System\\Apps\\MyleftCenter\\MyleftCenter.flg"
#define KUpdataListFlag	"C:\\System\\Apps\\MyleftCenter\\MyleftUpdataList.flg"
_LIT(KReadAboutFilename	,"C:\\System\\Apps\\MyleftCenter\\ReadAbout.txt");
_LIT (KBackPictrueFolderC, "C:\\System\\Apps\\MyleftCenter\\BackPic\\");	
_LIT (KBackPictrueFolderE, "E:\\MyLeft\\BackPic\\");
_LIT (CONFIG_FILENAME, "C:\\System\\Apps\\MyleftCenter\\Config.xml");
_LIT (READLOCAL_FILENAME, "C:\\System\\Apps\\MyleftCenter\\ReadLocalList.xml");
_LIT (ReadMask_FileName, "C:\\System\\Apps\\MyleftCenter\\ReadMaskList.xml");
_LIT (MusicLocal_FileName, "C:\\System\\Apps\\MyleftCenter\\MusicLocalList.xml");
_LIT (MusicMask_FileName, "C:\\System\\Apps\\MyleftCenter\\MusicMaskList.xml");
_LIT (DownLoad_FileName, "C:\\System\\Apps\\MyleftCenter\\DownLoadList.xml");
_LIT (DEFAULT_CONFIG_FILENAME, "C:\\System\\Apps\\MyleftCenter\\DefConfig.xml");
_LIT (OEM_FILENAME, "C:\\System\\Apps\\MyleftCenter\\oem.xml");
_LIT (KMusicMultiBitmapFilename, "\\System\\Apps\\MyleftCenter\\MyleftCenter.mbm");	
#endif

_LIT (API_SPECIALS, "specialsl");
_LIT (API_CHANNELS, "channels");
_LIT (API_CLASSIFIES, "classifies");	


#define KMain_Icon_Num		7
#define  KMainIcon_WH		65
#define KPopSelectIconH		26

#if defined(_PIC_176X220_)
#define KTitleHight_Len		25
#define KButtonHight_Len		26
#define KMainWight			176
#define  kMainHight			220
#define KTitleIcon_len		0
#define KTitleContent_len		0
#define KEdit_Width			110
#define KEdit_Hight			20
#define KPlayContrl_lrcH		25
#define KPlayContrl_listH		116
#define KPlayContrl_PicW		54
#define KReadinfo_ItemH		90
#define KDownload_ItemH		40
#elif defined(_PIC_240X320_)
#define KTitleHight_Len		25
#define KMainWight			240
#define  kMainHight			320
#define KTitleIcon_len		0
#define KTitleContent_len		50
#define KEdit_Width			93
#define KEdit_Hight			20
#define KPlayContrl_lrcH		25
#define KPlayContrl_listH		165
#define KPlayContrl_PicW		80
#define KReadinfo_ItemH		40
#define KDownload_ItemH		40
#elif defined(_PIC_320X240_)
#define KTitleHight_Len		30
#define KButtonHight_Len		30
#define KMainWight			320
#define  kMainHight			240
#define KTitleIcon_len		0
#define KTitleContent_len		0
#define KEdit_Width			180
#define KEdit_Hight			30
#define KPlayContrl_lrcH		25
#define KPlayContrl_listH		165
#define KPlayContrl_PicW		80
#define KReadinfo_ItemH		90
#define KDownload_ItemH		40
#elif defined(_PIC_352X416_)
#define KTitleHight_Len		28

#elif defined(_PIC_208X208_)
#define KTitleHight_Len		22

#endif

#if 0
#define KRgbTitle   		TRgb(104,40,16)
#define KRgbContainer   	TRgb(41,158,190)
#define KRgbContainerDeep   	TRgb(11,73,100)
#define KRgbContainerDeep2   	TRgb(11,73,100)
#define KRgbRect   	TRgb(16,77,104)
#define KRgbText   	TRgb(0,128,0)
#else
#define KRgbTitle   		TRgb(104,40,16)
//#define KRgbContainer2   	TRgb(225,225,225)
#define KRgbContainer   	TRgb(204,204,204)
#define KRgbContainerDeep   	TRgb(153,153,153)
#define KRgbRect   	TRgb(102,102,102)
#define KRgbText   	TRgb(2,2,2)
#endif

#ifndef _NOKIA_90_
#define FP_S_RELEASE			"5.0"
#define FP_L_RELEASE			_L("5.0")
#define FP_Major_Version		5
#define FP_Minor_Version		0
#else
#define FP_S_RELEASE			"5.0"
#define FP_L_RELEASE			_L("5.0")
#define FP_Major_Version		5
#define FP_Minor_Version		0
#endif


#endif //_FONDO_CONST_3RD_H_
