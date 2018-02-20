
#include <txtrich.h> // CRichText

#include "SinaInterfaceMgr.h"
#include "portab_symbian.h"
#include "CenterCore.h"
#include "const.h"
#include <MyleftCenter.rsg>

#include  "MyleftCenterViewReadList.h"
#include "MyleftCenterContainerReadList.h"
#include  "MyleftCenterViewSearch.h"
#include  "MyleftCenterViewMusic.h"
#include "MyleftCenterContainerMusic.h"
#include  "MyleftCenterContainerSearch.h" 
#include  "MyleftCenterViewMusicList.h"
#include "MyleftCenterContainerMusicList.h"
#include  "MyleftCenterViewMusicSearch.h"
#include  "MyleftCenterContainerMusicSearch.h" 
#include  "MyleftCenterViewDownload.h"
#include "MyleftCenterContainerDownload.h"

#ifdef __WINS__
const TMsvId KObservedFolderId = KMsvDraftEntryId;
#else
const TMsvId KObservedFolderId = KMsvGlobalInBoxIndexEntryId;
#endif


CSinaInterfaceMgr*  CSinaInterfaceMgr::NewL(CenterCore * owner)
{
	CSinaInterfaceMgr* self = new (ELeave) CSinaInterfaceMgr;
	CleanupStack::PushL (self);
	self->ConstructL (owner);
	CleanupStack::Pop ();
	return self;
}

 CSinaInterfaceMgr::~CSinaInterfaceMgr()
{
	ReleaseTimer();
	if(iMusicGetter)
		delete iMusicGetter;
	if(iDownLoadGetter)
		delete iDownLoadGetter;
	if( iImageFetcher )
	{
		iImageFetcher->CancelAllTask();
		delete iImageFetcher;
	}
	if(iMsvEntry)
		delete iMsvEntry;
	if(iSmsMtm)
		delete iSmsMtm;
	if(iClientMtmReg)
		delete iClientMtmReg;
	if(iMsvSession)
		delete iMsvSession;

}

void CSinaInterfaceMgr::ConstructL (CenterCore * owner) 
{
	if(owner)
		iCenterCore=owner;
}

TBool CSinaInterfaceMgr::IsFetchingMusicInfo()
{
	return iMusicGetter && iTaskType;
}

TInt CSinaInterfaceMgr::IsFetchingType()
{
	if(iMusicGetter && iTaskType)
		return iTaskType;
	else 
		return 0;
}

TInt CSinaInterfaceMgr::IsDownLoading()
{
	if(iDownLoadGetter && iDownTask)
		return iDownTask;
	else 
		return 0;
}

void CSinaInterfaceMgr::CancelFetchMusicInfo()
{
	if( iMusicGetter && iTaskType )
	{
		FC_Log("CSinaInterfaceMgr::CancelFetchMusicInfo");
		iMusicGetter->CancelFetch();
		iTaskType = 0;
	}
	StopObserveSms();
}

void CSinaInterfaceMgr::CancelFetchDownload()
{
	if( iDownLoadGetter&& iDownTask)
	{
		FC_Log("CSinaInterfaceMgr::CancelFetchDownload");
		iDownLoadGetter->CancelFetch();
		iDownTask = 0;
	}
	StopObserveSms();
}

void CSinaInterfaceMgr::OnDismissDialogL( TInt aButtonId, TBool& aCanExit )
{
	if( iMusicGetter && iTaskType )
	{
		FC_Log("CSinaInterfaceMgr::OnDismissDialogL");
		iMusicGetter->CancelFetch();
		iTaskType = 0;
	}
}
void CSinaInterfaceMgr::GetLengthAndSize( TUint& aLen, TUint& aSize )
{
	if(iDownLoadGetter)
		iDownLoadGetter->GetLengthAndSize((TInt&)aLen, (TInt&)aSize);

}

TInt CSinaInterfaceMgr::LaunchGetTestUrl( const TDesC& aUrl)
{
	FC_Log( "CSinaInterfaceMgr::LaunchGetTestUrl: ");
	
	if(iTaskType )
	{
		return 0;
	}
	
	if( !iMusicGetter )
	{
		iMusicGetter = CMusicContentGetter::NewL(CenterCore::GetInstance()->_http_iap_id);
	}

	TInt err = KErrNone;
	HBufC* urlAddress_hbuf = HBufC::NewL( 512 );
	TPtr urlAddress = urlAddress_hbuf->Des();

	urlAddress.Copy( aUrl);

	_LIT (Funsmusic_TestUrl, "E:\\Funsmusic_TestUrl_Result.fsx");
	err = iMusicGetter->FetchAndSaveToFile( urlAddress,Funsmusic_TestUrl, this, EFalse );

	delete urlAddress_hbuf;

	if( err==KErrNone )
	{
		iTaskType = 5555;
		iCenterCore->ShowWaitNote(iCenterCore->TextResource(R_TEXT_SINAMGR_GETTING_DATA));

	}
	else
	{
		iTaskType = 0;
	}
	return err;
}
/*
TInt CSinaInterfaceMgr::LaunchGetFile(const TDesC& aName,const TDesC& aUrl, TInt _typeId)
{
	FC_Logsp( "CSinaInterfaceMgr::LaunchGetPlayerlrc: ");

	if(iDownTask )
	{
		CenterCore::ShowConfirmationMessage(iCenterCore->TextResource(R_TEXT_SINAMGR_GETTING_OTHER_DATA));
		return -1;
	}
	if( !iDownLoadGetter )
	{
		iDownLoadGetter = CMusicContentGetter::NewL(CenterCore::GetInstance()->_http_iap_id);
	}

	TInt err = KErrNone;
	
	HBufC* urlAddress_hbuf = HBufC::NewL( 512 );
	TPtr urlAddress = urlAddress_hbuf->Des();
	idownloadDes.SetLength(0);
	
	if(iLoadSongPath.Length())
	{
		idownloadDes.Copy(iLoadSongPath);
		iLoadSongPath.SetLength(0);
	}
	else
		iCenterCore->Read_GetDownTextPath(idownloadDes);
	idownloadDes.Append(aName);
	urlAddress.Copy(aUrl);
		
	err = iDownLoadGetter->FetchAndSaveToFile( urlAddress,idownloadDes, this, EFalse );
	delete urlAddress_hbuf;

	if( err==KErrNone )
	{
		iDownTask = 1001;
		FC_Logsp( "CSinaInterfaceMgr::LaunchGetReadFile: iDownTask=1001");
	//	CFunWaitDialog::ShowDlgLD( _L("LaunchGetPlayerlrc!"),  _L("Please Wait!"),iFondoGlobal->m_appUI->UiStyler(), 0 );

	}
	else
	{
		FC_Logsp( "CSinaInterfaceMgr::LaunchGetReadFile:  err=%d",(int)err);
		iDownTask = 0;
	}
	return err;
}
*/
TInt CSinaInterfaceMgr::LaunchGetReadFile(TInt _Id,const TDesC& aName,TInt _typeId)
{
	FC_Logsp( "CSinaInterfaceMgr::LaunchGetPlayerlrc: ");

	if(iDownTask )
	{
		CenterCore::ShowConfirmationMessage(iCenterCore->TextResource(R_TEXT_SINAMGR_GETTING_OTHER_DATA));
		return -1;
	}
	if( !iDownLoadGetter )
	{
		iDownLoadGetter = CMusicContentGetter::NewL(CenterCore::GetInstance()->_http_iap_id);
	}

	TInt err = KErrNone;
	
	HBufC* urlAddress_hbuf = HBufC::NewL( 512 );
	TPtr urlAddress = urlAddress_hbuf->Des();
	idownloadDes.SetLength(0);
	
	if(iLoadSongPath.Length())
	{
		idownloadDes.Copy(iLoadSongPath);
		iLoadSongPath.SetLength(0);
	}
	else
	{
		if(_typeId==MYLEFG_API_TYPE_TXT)
			iCenterCore->Read_GetDownTextPath(idownloadDes);
		else if(_typeId==MYLEFG_API_TYPE_MUSIC|| _typeId==MYLEFG_API_TYPE_LRC)
			iCenterCore->Music_GetDownMP3Path(idownloadDes);
		else if(_typeId==MYLEFG_API_TYPE_PIC)
			iCenterCore->GetDownPicPath(idownloadDes);
	}
	idownloadDes.Append(aName);
	/*
	if(_typeId==MYLEFG_API_TYPE_MUSIC)
		idownloadDes.Append(_L(".mp3"));
	else if(_typeId==MYLEFG_API_TYPE_TXT)
		idownloadDes.Append(_L(".txt"));
	else
		idownloadDes.Append(_L(".lrc"));
*/	
	urlAddress.Copy( iCenterCore->GetDownLoadAddress() );
	urlAddress.Append(_L("/id/"));
	urlAddress.AppendNum(_Id);

	if(_typeId==MYLEFG_API_TYPE_LRC)
			urlAddress.Append(_L("/filetype/lrc"));
	else if(_typeId==MYLEFG_API_TYPE_PIC)
			urlAddress.Append(_L("/filetype/picture"));
		
	err = iDownLoadGetter->FetchAndSaveToFile( urlAddress,idownloadDes, this, EFalse );
	delete urlAddress_hbuf;

	if( err==KErrNone )
	{
		iDownTask = 1001;
		FC_Logsp( "CSinaInterfaceMgr::LaunchGetReadFile: iDownTask=1001");
	//	CFunWaitDialog::ShowDlgLD( _L("LaunchGetPlayerlrc!"),  _L("Please Wait!"),iFondoGlobal->m_appUI->UiStyler(), 0 );

	}
	else
	{
		FC_Logsp( "CSinaInterfaceMgr::LaunchGetReadFile:  err=%d",(int)err);
		iDownTask = 0;
	}
	return err;
}

TInt CSinaInterfaceMgr::LaunchGetBangdanClass(TInt _classes,TInt _typeID)
{
	FC_Logsp( "CSinaInterfaceMgr::LaunchGetBangdan: ");

	if(iTaskType)
	{
		CenterCore::ShowConfirmationMessage(iCenterCore->TextResource(R_TEXT_SINAMGR_GETTING_OTHER_DATA));
		return -1;
	}
	if( !iMusicGetter)
	{
		iMusicGetter = CMusicContentGetter::NewL(CenterCore::GetInstance()->_http_iap_id);
	}

	TInt err = KErrNone;
	
	HBufC* urlAddress_hbuf = HBufC::NewL( 512 );
	TPtr urlAddress = urlAddress_hbuf->Des();
	
	urlAddress.Copy( iCenterCore->GetMainpageAddress() );
	urlAddress.Append(_L("/"));
	if(_classes==0)
		urlAddress.Append(API_SPECIALS);
	else if(_classes==1)
		urlAddress.Append(API_CHANNELS);
	else //if(_classes==2)
		urlAddress.Append(API_CLASSIFIES);
	urlAddress.Append(_L("/type/"));
	urlAddress.AppendNum(_typeID);
	
	err = iMusicGetter->FetchAndSaveToDesc( urlAddress, this, EFalse );
	delete urlAddress_hbuf;

	if( err==KErrNone )
	{
		iTaskType = 0001;
		FC_Logsp( "CSinaInterfaceMgr::LaunchGetBangdanClass: iTaskType=0001");
		iCenterCore->ShowWaitNote(iCenterCore->TextResource(R_TEXT_SINAMGR_GETTING_DATA));

	}
	else
	{
		FC_Logsp( "CSinaInterfaceMgr::LaunchGetBangdanClass:  err=%d",(int)err);
		iTaskType = 0;
	}
	return err;
}
TInt CSinaInterfaceMgr::ParseReadBangdanClass(TDes8& aText)
{
	FC_Logsp("CSinaInterfaceMgr::ParseReadBangdanClass: ");

	TInt ret = 0;
	sxml *sXml = new sxml;
	TRAPD( r, sXml->load_string( (const char*)aText.PtrZ() ) );
	if( r!=KErrNone )
	{
		delete sXml;
		return 0;
	}

	sxml_node* node, *node1, *node2, *node3;
	TBuf<32> _sina_userid;
	node = sXml->find_path( "result/flag" );
	if( node )
	{
		int intResult = node->get_value().as_string().as_int();
		if( intResult==1 )
		{
			TBuf<36> event;
			TInt classes=0;
			node = sXml->find_path( "result/event" );
			if (node) 
				CenterCore::SaveNodeValueAsDesc( node, event);	

			if(event==API_SPECIALS)
				classes=MYLEFG_API_SPECIALS;
			else if(event==API_CHANNELS)
				classes=MYLEFG_API_CHANNELS;
			else //API_CLASSIFIES
				classes=MYLEFG_API_CLASSIFIES;

			// uid
			node = sXml->find_path( "result" );
			int i, n = node->child_count();

			for( i=0; i<n; i++ )
			{
				node1 = node->childs(i);
				if( node1->_name==ansistring("item") )
				{
					TInt id=-1,type_id;
					TInt count=0;
					TBuf<32> name_hbuf;

					if( !FindNodeAndSaveToInt( node1, "item/id", id) )
					{
						break;
					}
					
					if( !FindNodeAndSaveToInt( node1, "item/type_id", type_id) )
					{
						break;
					}
					
					if( !FindNodeAndSaveToDesc( node1, "item/name", name_hbuf) )
					{
						break;
					}

					if(type_id==1)//is music
					{
						if(classes==MYLEFG_API_CHANNELS)
							iCenterCore->Music_ChannelsAppend(type_id,id,name_hbuf);
						else  if(classes==MYLEFG_API_CLASSIFIES)
							iCenterCore->Music_ClassifiesAppend(type_id,id,name_hbuf);
					}
					else if(type_id==2)//is txt
					{
						if(classes==MYLEFG_API_CHANNELS)
							iCenterCore->Read_ChannelsAppend(type_id,id,name_hbuf);
						else  if(classes==MYLEFG_API_CLASSIFIES)
							iCenterCore->Read_ClassifiesAppend(type_id,id,name_hbuf);
					}
				}
			}
		}
	}

	delete sXml;
	iCenterCore->HideWaitNote();
	if(iCenterCore->m_appUI->viewReadList->iContainer)
	{		
		iCenterCore->m_appUI->viewReadList->iContainer->UpdateListItemsL();
	}
	else if(iCenterCore->m_appUI->viewMusicList->iContainer)
	{		
		iCenterCore->m_appUI->viewMusicList->iContainer->UpdateListItemsL();
	}
	
	FC_Logsp("CSinaInterfaceMgr::ParseReadBangdanClass: --------OK----");
	return ret;
}

// http://mobile.myleft.org/api/content/type/1/channel/1
TInt CSinaInterfaceMgr::LaunchGetList( TInt _id,TInt _typeid,TInt _class)
{
	FC_Logsp( "CSinaInterfaceMgr::LaunchGetBangdan: %d,%d,%d ",_id,_typeid,_class);

	if(iTaskType)
	{
		CenterCore::ShowConfirmationMessage(iCenterCore->TextResource(R_TEXT_SINAMGR_GETTING_OTHER_DATA));
		return -1;
	}
	if( !iMusicGetter)
	{
		iMusicGetter = CMusicContentGetter::NewL(CenterCore::GetInstance()->_http_iap_id);
	}

	TInt err = KErrNone;
	
	HBufC* urlAddress_hbuf = HBufC::NewL( 512 );
	TPtr urlAddress = urlAddress_hbuf->Des();
	
	urlAddress.Copy( iCenterCore->GetMainpageAddress() );
	urlAddress.Append(_L("/content/type/"));
	urlAddress.AppendNum(_typeid);

	if(_class==MYLEFG_API_SPECIALS)
		urlAddress.Append(_L("/special/"));
	else if(_class==MYLEFG_API_CHANNELS)
		urlAddress.Append(_L("/channel/"));
	else if(_class==MYLEFG_API_CLASSIFIES)
		urlAddress.Append(_L("/classify/"));
	urlAddress.AppendNum(_id);
	
	err = iMusicGetter->FetchAndSaveToDesc( urlAddress, this, EFalse );
	delete urlAddress_hbuf;

	if( err==KErrNone )
	{
		if(_typeid==MYLEFG_API_TYPE_MUSIC)
			iTaskType = 0021;
		else//MYLEFG_API_TYPE_TXT
			iTaskType = 0002;
		FC_Logsp( "CSinaInterfaceMgr::LaunchGetBangdan: iTaskType=0002");
		iCenterCore->ShowWaitNote(iCenterCore->TextResource(R_TEXT_SINAMGR_GETTING_DATA));

	}
	else
	{
		FC_Logsp( "CSinaInterfaceMgr::LaunchGetBangdan:  err=%d",(int)err);
		iTaskType = 0;
	}
	return err;
}

TInt CSinaInterfaceMgr::ParseGetList(TDes8& aText)
{
	FC_Logsp("CSinaInterfaceMgr::ParseGetList: ");

	TInt ret = 0;
	sxml *sXml = new sxml;
	TRAPD( r, sXml->load_string( (const char*)aText.PtrZ() ) );
	if( r!=KErrNone )
	{
		delete sXml;
		return 0;
	}

	sxml_node* node, *node1, *node2, *node3;
	TBuf<32> _sina_userid;
	node = sXml->find_path( "result/flag" );
	if( node )
	{
		int intResult = node->get_value().as_string().as_int();
		if( intResult==1 )
		{
			// uid
			if(iTaskType ==0002)
				iCenterCore->Read_bangdListReset();
			else if(iTaskType ==0021)
				iCenterCore->Music_bangdListReset();
			
			FC_Logsp("CSinaInterfaceMgr::ParseGetList: ----intResult==1--iTaskType=%d----",iTaskType);
			node = sXml->find_path( "result" );
			int i, n = node->child_count();

			for( i=0; i<n; i++ )
			{
				node1 = node->childs(i);
				if( node1->_name==ansistring("item") )
				{
					TInt bookId=-1;
					TBuf<32> name_hbuf;
					TBuf<32> auth_hbuf;
					TInt  	pictureid;
					TInt 		lrcid;
					
					if( !FindNodeAndSaveToInt( node1, "item/id", bookId) )
					{
						break;
					}
					if( !FindNodeAndSaveToDesc( node1, "item/title", name_hbuf) )
					{
						break;
					}
					if( !FindNodeAndSaveToDesc( node1, "item/author_name", auth_hbuf) )
					{
						break;
					}
					FindNodeAndSaveToInt( node1, "item/picture", pictureid);
					FindNodeAndSaveToInt( node1, "item/lrc", lrcid);

					if(iTaskType ==0002)
						iCenterCore->Read_bangdListAppend(bookId,name_hbuf,auth_hbuf,pictureid);
					else if(iTaskType ==0021)
						iCenterCore->Music_bangdListAppend(bookId,name_hbuf,auth_hbuf,pictureid,lrcid);
					
					FC_Logsp("CSinaInterfaceMgr::ParseGetList: ----append ==%d------",i);
				}
			}
		}
	}

	delete sXml;
	iCenterCore->HideWaitNote();
	if(iCenterCore->m_appUI->viewReadList->iContainer)
	{		
		iCenterCore->m_appUI->viewReadList->iContainer->UpdateListItemsL();
	}
	if(iCenterCore->m_appUI->viewMusicList->iContainer)
	{		
		iCenterCore->m_appUI->viewMusicList->iContainer->UpdateListItemsL();
	}
	
	FC_Logsp("CSinaInterfaceMgr::ParseGetList: --------OK----");
	return ret;
}

TInt CSinaInterfaceMgr::LaunchSearchRead( const TDesC& _szKey)
{
	return LaunchSearch(2, _szKey,0,3);
}

TInt CSinaInterfaceMgr::LaunchSearchReadAppend(const TDesC& _szKey,TInt _szPage)
{
	return LaunchSearch(2, _szKey, _szPage,4);
}

TInt CSinaInterfaceMgr::LaunchSearchMusic(const TDesC& _szKey)
{
	return LaunchSearch(  1, _szKey,0,5);
}

TInt CSinaInterfaceMgr::LaunchSearchMusicAppend(const TDesC& _szKey,TInt _szPage)
{
	return LaunchSearch(1, _szKey, _szPage,6);
}


// http://myleft.org/center/index.php/api/readsearch/type/1/page/1/limit/5?key=XXX
TInt CSinaInterfaceMgr::LaunchSearch( TInt _szclass,const TDesC& _szKey,TInt _szPage,TInt _iTaskType)
{
	FC_Logsp( "CSinaInterfaceMgr::LaunchGetReadSearch: ");

	if(iTaskType)
	{
		CenterCore::ShowConfirmationMessage(iCenterCore->TextResource(R_TEXT_SINAMGR_GETTING_OTHER_DATA));
		return -1;
	}
	if( !iMusicGetter)
	{
		iMusicGetter = CMusicContentGetter::NewL(CenterCore::GetInstance()->_http_iap_id);
	}

	TInt err = KErrNone;
	
	HBufC* urlAddress_hbuf = HBufC::NewL( 512 );
	TPtr urlAddress = urlAddress_hbuf->Des();
	HBufC* urlKeyEncoded_hbuf = HBufC::NewL( 128 );
	TPtr urlKeyEncoded = urlKeyEncoded_hbuf->Des();
	
	urlAddress.Copy( iCenterCore->GetMainpageAddress() );
	urlAddress.Append(_L("/content"));
	urlAddress.Append(_L("/type/"));
	urlAddress.AppendNum(_szclass);
	urlAddress.Append(_L("/page/"));
	urlAddress.AppendNum(_szPage);
	urlAddress.Append(_L("/limit/"));
	urlAddress.AppendNum(One_Page_Lenth);

	if(_szKey.Length())
	{
		iCenterCore->UrlParamEncode( _szKey, urlKeyEncoded, ETrue );

		urlAddress.Append(_L("?key="));
		urlAddress.Append(urlKeyEncoded);
	}

	err = iMusicGetter->FetchAndSaveToDesc( urlAddress, this, EFalse );
	delete urlAddress_hbuf;
	delete urlKeyEncoded_hbuf;
	if( err==KErrNone )
	{
		iTaskType = _iTaskType;
		FC_Logsp( "CSinaInterfaceMgr::LaunchGetReadSearch: iTaskType=%d",_iTaskType);
		iCenterCore->ShowWaitNote(iCenterCore->TextResource(R_TEXT_SINAMGR_GETTING_DATA));

	}
	else
	{
		FC_Logsp( "CSinaInterfaceMgr::LaunchGetReadSearch:  err=%d",(int)err);
		iTaskType = 0;
	}
	return err;
}

TInt CSinaInterfaceMgr::ParseReadSearch(TDes8& aText)
{
	FC_Logsp("CSinaInterfaceMgr::ParseReadSearch: ");

	TInt ret = 0;
	TBool hasResult=0;
	sxml *sXml = new sxml;
	TRAPD( r, sXml->load_string( (const char*)aText.PtrZ() ) );
	if( r!=KErrNone )
	{
		delete sXml;
		FC_Logsp( "CSinaInterfaceMgr::ParseReadSearch: sXml->load_string error");
		return 0;
	}

	sxml_node* node, *node1, *node2, *node3;
	TBuf<32> _sina_userid;
	node = sXml->find_path( "result/flag" );
	if( node )
	{
		int intResult = node->get_value().as_string().as_int();
		FC_Logsp( "CSinaInterfaceMgr::ParseReadSearch: intResult=%d",intResult);
		if( intResult==1 )
		{
			// uid
			if(iTaskType==0003)  //  read research
			{
				iCenterCore->m_appUI->viewSearch->iResultPage=0;
				iCenterCore->Read_SearchListReset();
			}
			else if(iTaskType==0005)  //music research
			{
				iCenterCore->m_appUI->viewMusicSearch->iResultPage=0;
				iCenterCore->Music_SearchListReset();
			}


			
			node = sXml->find_path( "result" );
			int i, n = node->child_count();

			for( i=0; i<n; i++ )
			{
				node1 = node->childs(i);
				if( node1->_name==ansistring("item") )
				{
					TInt bookId=-1;
					TBuf<32> name_hbuf;
					TBuf<32> auth_hbuf;
					TInt Picid;
					TInt Lrcid;
					if( !FindNodeAndSaveToInt( node1, "item/id", bookId) )
					{
						break;
					}
					
					if( !FindNodeAndSaveToDesc( node1, "item/title", name_hbuf) )
					{
						break;
					}

				
					if( !FindNodeAndSaveToDesc( node1, "item/author_name", auth_hbuf) )
					{
						break;
					}
					FindNodeAndSaveToInt( node1, "item/picture", Picid);
					FindNodeAndSaveToInt( node1, "item/lrc", Lrcid);
						
					hasResult=1;

					if(iTaskType==0003)  //  read research
						iCenterCore->Read_SearchListAppend(bookId,name_hbuf,auth_hbuf,Picid);
					else if(iTaskType==0005)
						iCenterCore->Music_SearchListAppend(bookId,name_hbuf,auth_hbuf,Picid,Lrcid);
				}
			}
		}
	}

	delete sXml;

	if(iTaskType==0003)  //  read research
	{
		if(hasResult)
		{
			iCenterCore->m_appUI->viewSearch->iResultPage++;
			iCenterCore->m_appUI->viewSearch->iCurPage=iCenterCore->m_appUI->viewSearch->iResultPage;
		}
		iCenterCore->HideWaitNote();
		if(iCenterCore->m_appUI->viewSearch->iContainer)
		{		
			iCenterCore->m_appUI->viewSearch->iContainer->SetPageNum(iCenterCore->m_appUI->viewSearch->iResultPage);
			iCenterCore->m_appUI->viewSearch->iContainer->UpDateHotlist();
		}
	}
	else if(iTaskType==0005)
	{
		if(hasResult)
		{
			iCenterCore->m_appUI->viewMusicSearch->iResultPage++;
			iCenterCore->m_appUI->viewMusicSearch->iCurPage=iCenterCore->m_appUI->viewMusicSearch->iResultPage;
		}
		iCenterCore->HideWaitNote();
		if(iCenterCore->m_appUI->viewMusicSearch->iContainer)
		{		
			iCenterCore->m_appUI->viewMusicSearch->iContainer->SetPageNum(iCenterCore->m_appUI->viewMusicSearch->iResultPage);
			iCenterCore->m_appUI->viewMusicSearch->iContainer->UpDateSearchlist();
		}
	}
	
	FC_Logsp("CSinaInterfaceMgr::ParseSearch: ---iTaskType=%d-----OK----",iTaskType);
	return ret;
}

//////////////////////////////////////////////////////////////////////////
TInt CSinaInterfaceMgr::LaunchSearchLrcUrl(const TDesC& aSigner, const TDesC& aName,const TDesC& album,const TDesC& aDesPath )
{
//	iFondoGlobal->iLogger->Log( _L("CSinaInterfaceMgr::LaunchSearch: ") );

//	iFondoGlobal->lrc_listReset();
	if(iTaskType )
	{
	//	FPlayerCore::ShowConfirmationMessage(iFondoGlobal->TextResource(R_TEXT_SINAMGR_GETTING_OTHER_DATA));
		return -1;
	}

	if( !iMusicGetter )
	{
		iMusicGetter = CMusicContentGetter::NewL(CenterCore::GetInstance()->_http_iap_id);
	}

	iLoadSongPath.Copy(aDesPath);

	TInt err = KErrNone;
	TBuf<125> aKeyword;
	HBufC* urlAddress_hbuf = HBufC::NewL( 512 );
	TPtr urlAddress = urlAddress_hbuf->Des();
	HBufC* urlKeyEncoded_hbuf = HBufC::NewL( 128 );
	TPtr urlKeyEncoded = urlKeyEncoded_hbuf->Des();

	if(aName.Length())
	{
		aKeyword.Copy(aName);
		iCenterCore->UrlParamEncode( aKeyword, urlKeyEncoded, ETrue );
	}
	urlAddress.Copy( iCenterCore->GetMainpageAddress() );
	urlAddress.Append( _L("/content/type/1/filetype/lrc") );
	urlAddress.Append( _L("?key=") );
	urlAddress.Append( urlKeyEncoded );

	if(aSigner.Length())
	{
		aKeyword.Copy(aSigner);
		iCenterCore->UrlParamEncode( aKeyword, urlKeyEncoded, ETrue );
		urlAddress.Append( _L("&author=") );
		urlAddress.Append( urlKeyEncoded );
	}

	err = iMusicGetter->FetchAndSaveToDesc( urlAddress, this, EFalse );

	delete urlAddress_hbuf;
	delete urlKeyEncoded_hbuf;

	if( err==KErrNone )
	{
		iTaskType = 8;
	//	CFunWaitDialog::RunDlgLD( _L("auto LaunchPlayingLrcUrl!"),  _L("please wait!"),iFondoGlobal->m_appUI->UiStyler(), 0 );
	}
	else
	{
		iTaskType = 0;
	}
	return err;
}


TInt CSinaInterfaceMgr::ParseSearchLrc(TDes8& aText)
{
	FC_Log("CSinaInterfaceMgr::ParseSearchLrc: ");

	TInt ret = 0;
	sxml *sXml = new sxml;

	iCenterCore->Music_LrcListReset();
	TRAPD( r, sXml->load_string( (const char*)aText.PtrZ() ) );
	if( r!=KErrNone )
	{
		iCenterCore->HideWaitNote();
		if(iCenterCore->m_appUI->viewMusic)
			iCenterCore->m_appUI->viewMusic->ChangeShowMode(2);

		delete sXml;
		return 0;
	}

	sxml_node* node, *node1, *node2, *node3;
	TBuf<32> _sina_userid;
	node = sXml->find_path( "result/flag" );
	if( node )
	{
		int intResult = node->get_value().as_string().as_int();
		FC_Logsp( "CSinaInterfaceMgr::ParseSearchLrc: intResult=%d",intResult);
		if( intResult==1 )
		{			
			node = sXml->find_path( "result" );
			int i, n = node->child_count();

			for( i=0; i<n; i++ )
			{
				node1 = node->childs(i);
				if( node1->_name==ansistring("item") )
				{
					TInt bookId=-1;
					TBuf<32> name_hbuf;
					TBuf<32> auth_hbuf;
					
					if( !FindNodeAndSaveToInt( node1, "item/id", bookId) )
					{
						break;
					}
					
					if( !FindNodeAndSaveToDesc( node1, "item/title", name_hbuf) )
					{
						break;
					}

				
					if( !FindNodeAndSaveToDesc( node1, "item/author_name", auth_hbuf) )
					{
						break;
					}

					iCenterCore->Music_LrcListAppend(bookId,name_hbuf,auth_hbuf);
				}
			}
		}
	}
//	
	delete sXml;
	iCenterCore->HideWaitNote();
	
	if(iCenterCore->m_musiclrclist->Count())
	{
		iCenterCore->m_appUI->viewMusic->SetLrcLibleShowId(ELrcLibShow_HaveLRC); //ELrcLibShow_HaveLRC
		iCenterCore->m_appUI->viewMusic->ChangeShowMode(2);
	}
	else
	{
		iCenterCore->m_appUI->viewMusic->SetLrcLibleShowId(ELrcLibShow_NoAutoLRC); //ELrcLibShow_NoAutoLRC
	}
	

	FC_Log("CSinaInterfaceMgr::ParsePlayingInfo: --------OK----");
	return ret;
}


TInt CSinaInterfaceMgr::FindNodeAndSaveToDesc(sxml_node* _root_node, const char* _find, TDes& _string)
{
	_string.SetLength(0);
	sxml_node* node = _root_node->find_path( _find );
	if( !node )
		return 0;
	ansistring nodeString = node->get_value().as_string();
	if( !nodeString.is_null() && nodeString.length()>0 )
	{
	//	FC_Logsp("FindNodeAndSaveToDesc:%s",nodeString.as_char());
		if( nodeString.length()<=_string.MaxLength() )
			CenterCore::SaveNodeValueAsDesc( node, _string );
		else
		{
			TPtrC8 _value8;
			_value8.Set( (const TUint8*)nodeString.as_char(), _string.MaxLength() );
			FC_Utf8ToUnicodeL( _string, _value8 );
		}
	}

	return 1;
}
TInt CSinaInterfaceMgr::FindNodeAndSaveToInt(sxml_node* _root_node, const char* _find, TInt& _int)
{
	_int = 0;
	sxml_node* node = _root_node->find_path( _find );
	if( !node )
		return 0;
	CenterCore::SaveNodeValueAsInt( node, _int );
	return 1;
}

/*
TInt CSinaInterfaceMgr::ParseUserInfoAfterLogon(TDes8& aText)
{
	FC_Log("CSinaInterfaceMgr::ParseUserInfoAfterLogon: ");

	TInt ret = 0;
	sxml *sXml = new sxml;
	TRAPD( r, sXml->load_string( (const char*)aText.PtrZ() ) );
	if( r!=KErrNone )
	{
		delete sXml;
		return 0;
	}

	sxml_node* node, *node1, *node2, *node3;
	TBuf<32> _sina_userid;
	node = sXml->find_path( "fpv/result" );
	if( node )
	{
		int intResult = node->get_value().as_string().as_int();
		if( intResult==1 )
		{
			// uid
			iFondoGlobal->register_success=1;
			node = sXml->find_path( "fpv/path/YueInitModel/id" );
			if( node )
			{
				FPlayerCore::SaveNodeValueAsDesc( node,_sina_userid );
				FPlayerCore::SaveNodeValueAsInt( node,iFondoGlobal->_sina_user_loc_id);
				iFondoGlobal->_sina_userid.Copy(_sina_userid);
				ret = 1;		// return success
				node = sXml->find_path( "fpv/path/YueInitModel/userAlbumList" );
				if( node )
				{
					iFondoGlobal->bd_listReset();
					int i, n = node->child_count();

					for( i=0; i<n; i++ )
					{
						node1 = node->childs(i);
						if( node1->_name==ansistring("UserAlbum") )
						{
							ParseUserAlbumInfo( node1, ETrue );
						}
					}

				}
			}			
		}
	}

	delete sXml;

	if(iFondoGlobal->m_appUI->IsForeground())
	{
		if(iFondoGlobal->m_appUI->viewLoad->iContainer)
		{
			iFondoGlobal->m_appUI->viewLoad->iContainer->StopAndCancle();
			iFondoGlobal->m_appUI->ActivateLocalViewL(KViewId);
		}
		iFondoGlobal->m_appUI->deactiveLoadView=ETrue;
	}
	else
	{
		iFondoGlobal->m_appUI->deactiveLoadView=ETrue;
		//if(iFondoGlobal->m_appUI->viewLoad->iContainer)
		//{
		//	FC_Logsp("CSinaInterfaceMgr::iForeground is false: StopAndCancle ");
		//	iFondoGlobal->m_appUI->viewLoad->iContainer->StopAndCancle();
			iFondoGlobal->m_appUI->ActivateLocalViewL(KViewId);
		//}
		//FC_Logsp("CSinaInterfaceMgr::iForeground is false: ");
	}
	FC_Log("CSinaInterfaceMgr::ParseUserInfoAfterLogon: --------OK----");
	return ret;
}

TInt CSinaInterfaceMgr::ParseUserInfoAfterLogon2(TDes8& aText)
{
	FC_Log("CSinaInterfaceMgr::ParseUserInfoAfterLogon2: ");

	TInt ret = 0;
	sxml *sXml = new sxml;
	TRAPD( r, sXml->load_string( (const char*)aText.PtrZ() ) );
	if( r!=KErrNone )
	{
		delete sXml;
		return 0;
	}

	sxml_node* node, *node1, *node2, *node3;
	TBuf<32> _sina_userid;
	node = sXml->find_path( "fpv/result" );
	if( node )
	{
		int intResult = node->get_value().as_string().as_int();
		if( intResult==1 )
		{
			// uid
			iFondoGlobal->register_success=1;
			node = sXml->find_path( "fpv/path/YueInitModel/id" );
			if( node )
			{
				FPlayerCore::SaveNodeValueAsDesc( node,_sina_userid );
				FPlayerCore::SaveNodeValueAsInt( node,iFondoGlobal->_sina_user_loc_id);
				iFondoGlobal->_sina_userid.Copy(_sina_userid);
				ret = 1;		// return success
				node = sXml->find_path( "fpv/path/YueInitModel/userAlbumList" );
				if( node )
				{
					iFondoGlobal->bd_listReset();
					int i, n = node->child_count();

					for( i=0; i<n; i++ )
					{
						node1 = node->childs(i);
						if( node1->_name==ansistring("UserAlbum") )
						{
							ParseUserAlbumInfo( node1, ETrue );
						}
					}

				}
			}			
		}
	}

	delete sXml;

	if(iFondoGlobal->m_appUI->viewBdan->iContainer)
		iFondoGlobal->m_appUI->viewBdan->iContainer->UpdateListItemsL();
	FC_Log("CSinaInterfaceMgr::ParseUserInfoAfterLogon2: --------OK----");
	return ret;
}
void CSinaInterfaceMgr::ParseUserAlbumInfo( sxml_node* aAlbumNode, TBool aSave)
{
	FC_Log("CSinaInterfaceMgr::ParseUserAlbumInfo");
//	TBuf<100> aString;
	TBuf<16> sinaId;
	HBufC* name_hbuf = HBufC::NewL( 512 );
	TPtr name= name_hbuf->Des();
	int data;

	if( !FindNodeAndSaveToDesc( aAlbumNode, "UserAlbum/sinaId", sinaId) )
	{
		delete name_hbuf;
		return ;
	}
	
	if( !FindNodeAndSaveToDesc( aAlbumNode, "UserAlbum/name", name) )
	{
		delete name_hbuf;
		return ;
	}
	
	iFondoGlobal->bd_listAppend(sinaId,name);
		
	delete name_hbuf;
	FC_Log("CSinaInterfaceMgr::ParseUserAlbumInfo ()  OK");
}

TInt CSinaInterfaceMgr::ParseTop10list(TDes8& aText)
{
	FC_Log("CSinaInterfaceMgr::ParseTop10list: ");

	TInt ret = 0;
	sxml *sXml = new sxml;
	TRAPD( r, sXml->load_string( (const char*)aText.PtrZ() ) );
	if( r!=KErrNone )
	{
		delete sXml;
		iFondoGlobal->HideWaitNote();
		return 0;
	}

	sxml_node* node, *node1, *node2, *node3;
	TBuf<32> _sina_userid;
	node = sXml->find_path( "fpv/result" );
	if( node )
	{
		int intResult = node->get_value().as_string().as_int();
		if( intResult==1 )
		{
			ret = 1;	
			node = sXml->find_path( "fpv/path" );
			iFondoGlobal->bditem_listReset();
			int i, n = node->child_count();

			for( i=0; i<n; i++ )
			{
				node1 = node->childs(i);
				if( node1->_name==ansistring("ViewSong") )
				{
					HBufC* url_hbuf = HBufC::NewL( 100 );
					HBufC* name_hbuf = HBufC::NewL( 32 );
					HBufC* singer_hbuf = HBufC::NewL( 32 );
					HBufC* down_hbuf = HBufC::NewL( 100 );
					HBufC* sinaID_hbuf = HBufC::NewL(18);
					TPtr sinaID= sinaID_hbuf->Des();
					TPtr downUrl= down_hbuf->Des();
					TPtr url= url_hbuf->Des();
					TPtr name= name_hbuf->Des();
					TPtr singer= singer_hbuf->Des();
					int data;

					if( !FindNodeAndSaveToDesc( node1, "ViewSong/iaskurl", url) )
					{
						delete down_hbuf;
						delete singer_hbuf;
						delete url_hbuf;
						delete name_hbuf;
						delete sinaID_hbuf;
						break;
					}
					
					if( !FindNodeAndSaveToDesc( node1, "ViewSong/sinaId", sinaID) )
					{
						delete down_hbuf;
						delete singer_hbuf;
						delete url_hbuf;
						delete name_hbuf;
						delete sinaID_hbuf;
						break;
					}
				
					if( !FindNodeAndSaveToDesc( node1, "ViewSong/name", name) )
					{
						delete down_hbuf;
						delete singer_hbuf;
						delete url_hbuf;
						delete name_hbuf;
						delete sinaID_hbuf;
						break;
					}
					if( !FindNodeAndSaveToDesc( node1, "ViewSong/singerCName", singer) )
					{
						delete down_hbuf;
						delete singer_hbuf;
						delete url_hbuf;
						delete name_hbuf;
						delete sinaID_hbuf;
						break;
					}
					if( !FindNodeAndSaveToDesc( node1, "ViewSong/wapList/Wap/wapDownload", downUrl) )
					{
						delete down_hbuf;
						delete singer_hbuf;
						delete url_hbuf;
						delete name_hbuf;
						delete sinaID_hbuf;
						break;
					}
					
					iFondoGlobal->bditem_listAppend(url,sinaID,downUrl,name,singer);
					delete down_hbuf;
					delete singer_hbuf;
					delete url_hbuf;
					delete name_hbuf;
					delete sinaID_hbuf;
				}
			}

						
		}
	}

	iFondoGlobal->HideWaitNote();
	if(iFondoGlobal->m_appUI->viewBdan->iContainer)
		iFondoGlobal->m_appUI->viewBdan->iContainer->UpdateListItemsL();

	delete sXml;

	FC_Log("CSinaInterfaceMgr::ParseTop10list: --------OK----");
	return ret;
}

TInt CSinaInterfaceMgr::ParseSearchSong(TDes8& aText)
{
	FC_Log("CSinaInterfaceMgr::ParseSearchSong: ");

	TInt ret = 0;
	TBool hasResult=0;
	sxml *sXml = new sxml;
	TRAPD( r, sXml->load_string( (const char*)aText.PtrZ() ) );
	if( r!=KErrNone )
	{
		delete sXml;
		return 0;
	}

	sxml_node* node, *node1, *node2, *node3;
	TBuf<32> _sina_userid;
	node = sXml->find_path( "fpv/result" );
	if( node )
	{
		int intResult = node->get_value().as_string().as_int();
		if( intResult==1 )
		{
			ret = 1;	
			node = sXml->find_path( "fpv/path" );
			if(iTaskType==41)  //research
			{
				iFondoGlobal->m_search_maxPage=0;
				iFondoGlobal->search_listReset();
			}
			int i, n = node->child_count();

			for( i=0; i<n; i++ )
			{
				node1 = node->childs(i);
				if( node1->_name==ansistring("ViewSong") )
				{
					HBufC* url_hbuf = HBufC::NewL( 100 );
					HBufC* name_hbuf = HBufC::NewL( 32 );
					HBufC* singer_hbuf = HBufC::NewL( 32 );
					HBufC* down_hbuf = HBufC::NewL( 100 );
					HBufC* sinaID_hbuf = HBufC::NewL(18);
					TPtr sinaID= sinaID_hbuf->Des();
					TPtr downUrl= down_hbuf->Des();
					TPtr url= url_hbuf->Des();
					TPtr name= name_hbuf->Des();
					TPtr singer= singer_hbuf->Des();
					int data;

					if( !FindNodeAndSaveToDesc( node1, "ViewSong/iaskurl", url) )
					{
						delete down_hbuf;
						delete singer_hbuf;
						delete url_hbuf;
						delete name_hbuf;
						delete sinaID_hbuf;
						break;
					}
					
					if( !FindNodeAndSaveToDesc( node1, "ViewSong/sinaId", sinaID) )
					{
						delete down_hbuf;
						delete singer_hbuf;
						delete url_hbuf;
						delete name_hbuf;
						delete sinaID_hbuf;
						break;
					}
				
					if( !FindNodeAndSaveToDesc( node1, "ViewSong/name", name) )
					{
						delete down_hbuf;
						delete singer_hbuf;
						delete url_hbuf;
						delete name_hbuf;
						delete sinaID_hbuf;
						break;
					}
					
					if( !FindNodeAndSaveToDesc( node1, "ViewSong/singerCName", singer) )
					{
						delete down_hbuf;
						delete singer_hbuf;
						delete url_hbuf;
						delete name_hbuf;
						delete sinaID_hbuf;
						break;
					}
					
					if( !FindNodeAndSaveToDesc( node1, "ViewSong/wapList/Wap/wapDownload", downUrl) )
					{
						delete down_hbuf;
						delete singer_hbuf;
						delete url_hbuf;
						delete name_hbuf;
						delete sinaID_hbuf;
						break;
					}
					
					iFondoGlobal->search_listAppend(url,sinaID,downUrl,name,singer);
					delete down_hbuf;
					delete singer_hbuf;
					delete url_hbuf;
					delete name_hbuf;
					delete sinaID_hbuf;
					hasResult=1;
				}
			}

						
		}
	}
//	
	if(hasResult)
	{
		iFondoGlobal->m_search_maxPage++;
		iFondoGlobal->m_appUI->viewSearch->iCurPage=iFondoGlobal->m_search_maxPage;
	}
	iFondoGlobal->HideWaitNote();
	if(iFondoGlobal->m_appUI->viewSearch->iContainer)
	{		
		iFondoGlobal->m_appUI->viewSearch->iContainer->SetPageNum(iFondoGlobal->m_search_maxPage);
		iFondoGlobal->m_appUI->viewSearch->iContainer->UpDateHotlist();
	}

	delete sXml;

	FC_Log("CSinaInterfaceMgr::ParseSearchSong: --------OK----");
	return ret;
}

TInt CSinaInterfaceMgr::ParseManualPlayingInfo(TDes8& aText)
{
	FC_Log("CSinaInterfaceMgr::ParseManualPlayingInfo: ");

	TInt ret = 0;
	sxml *sXml = new sxml;

	iFondoGlobal->lrc_listReset();
	TRAPD( r, sXml->load_string( (const char*)aText.PtrZ() ) );
	if( r!=KErrNone )
	{
		FC_Log("CSinaInterfaceMgr::ParseManualPlayingInfo: 1111111111");
		iFondoGlobal->HideWaitNote();
		if(iFondoGlobal->m_appUI->view1)
			iFondoGlobal->m_appUI->view1->ChangeShowMode(2);

		delete sXml;
		return 0;
	}

	sxml_node* node, *node1, *node2, *node3;
	TBuf<32> _sina_userid;
	node = sXml->find_path( "fpv/result" );
	if( node )
	{
		int intResult = node->get_value().as_string().as_int();
		if( intResult==1 )
		{
			ret = 1;	
			node = sXml->find_path( "fpv/path" );
			
			int i, n = node->child_count();

			for( i=0; i<n; i++ )
			{
				node1 = node->childs(i);
				if( node1->_name==ansistring("ViewSong") )
				{
					HBufC* url_hbuf = HBufC::NewL( 100 );
					HBufC* name_hbuf = HBufC::NewL( 32 );
					HBufC* singer_hbuf = HBufC::NewL( 32 );
					HBufC* sinaID_hbuf = HBufC::NewL(32);
					TPtr sinaID= sinaID_hbuf->Des();
					TPtr url= url_hbuf->Des();
					TPtr name= name_hbuf->Des();
					TPtr singer= singer_hbuf->Des();
					int data;

					if( !FindNodeAndSaveToDesc( node1, "ViewSong/iaskurl", url) )
					{
						delete singer_hbuf;
						delete url_hbuf;
						delete name_hbuf;
						delete sinaID_hbuf;
						break;
					}
					if( !FindNodeAndSaveToDesc( node1, "ViewSong/sinaId", sinaID) )
					{
						delete singer_hbuf;
						delete url_hbuf;
						delete name_hbuf;
						delete sinaID_hbuf;
						break;
					}
					if( !FindNodeAndSaveToDesc( node1, "ViewSong/name", name) )
					{
						delete singer_hbuf;
						delete url_hbuf;
						delete name_hbuf;
						delete sinaID_hbuf;
						break;
					}
					if( !FindNodeAndSaveToDesc( node1, "ViewSong/singerCName", singer) )
					{
						delete singer_hbuf;
						delete url_hbuf;
						delete name_hbuf;
						delete sinaID_hbuf;
						break;
					}
					iFondoGlobal->lrc_listAppend(url,sinaID,KNullDesC,name,singer);
					delete singer_hbuf;
					delete url_hbuf;
					delete name_hbuf;
					delete sinaID_hbuf;
				}
			}

						
		}
	}
//	
	delete sXml;
	iFondoGlobal->HideWaitNote();
	
	if(iFondoGlobal->m_lrc_count)
	{
		iFondoGlobal->m_appUI->view1->SetLrcLibleShowId(ELrcLibShow_HaveLRC); //ELrcLibShow_HaveLRC
		iFondoGlobal->m_appUI->view1->ChangeShowMode(2);
	}
	else
	{
		iFondoGlobal->m_appUI->view1->SetLrcLibleShowId(ELrcLibShow_NoManuLRC); //ELrcLibShow_NoManuLRC
	}
	FC_Log("CSinaInterfaceMgr::ParseManualPlayingInfo: OK");

	return ret;
}

TInt CSinaInterfaceMgr::ParsePlayingInfo(TDes8& aText)
{
	FC_Log("CSinaInterfaceMgr::ParsePlayingInfo: ");

	TInt ret = 0;
	sxml *sXml = new sxml;

	iFondoGlobal->lrc_listReset();
	TRAPD( r, sXml->load_string( (const char*)aText.PtrZ() ) );
	if( r!=KErrNone )
	{
		iFondoGlobal->HideWaitNote();
		if(iFondoGlobal->m_appUI->view1)
			iFondoGlobal->m_appUI->view1->ChangeShowMode(2);

		delete sXml;
		return 0;
	}

	sxml_node* node, *node1, *node2, *node3;
	TBuf<32> _sina_userid;
	node = sXml->find_path( "fpv/result" );
	if( node )
	{
		int intResult = node->get_value().as_string().as_int();
		if( intResult==1 )
		{
			ret = 1;	
			node = sXml->find_path( "fpv/path" );
			
			int i, n = node->child_count();

			for( i=0; i<n; i++ )
			{
				node1 = node->childs(i);
				if( node1->_name==ansistring("ViewSong") )
				{
					HBufC* url_hbuf = HBufC::NewL( 100 );
					HBufC* name_hbuf = HBufC::NewL( 32 );
					HBufC* singer_hbuf = HBufC::NewL( 32 );
					HBufC* sinaID_hbuf = HBufC::NewL(32);
					TPtr sinaID= sinaID_hbuf->Des();
					TPtr url= url_hbuf->Des();
					TPtr name= name_hbuf->Des();
					TPtr singer= singer_hbuf->Des();
					int data;

					if( !FindNodeAndSaveToDesc( node1, "ViewSong/iaskurl", url) )
					{
						delete singer_hbuf;
						delete url_hbuf;
						delete name_hbuf;
						delete sinaID_hbuf;
						break;
					}
					
					if( !FindNodeAndSaveToDesc( node1, "ViewSong/sinaId", sinaID) )
					{
						delete singer_hbuf;
						delete url_hbuf;
						delete name_hbuf;
						delete sinaID_hbuf;
						break;
					}
				
					if( !FindNodeAndSaveToDesc( node1, "ViewSong/name", name) )
					{
						delete singer_hbuf;
						delete url_hbuf;
						delete name_hbuf;
						delete sinaID_hbuf;
						break;
					}
					
					if( !FindNodeAndSaveToDesc( node1, "ViewSong/singerCName", singer) )
					{
						delete singer_hbuf;
						delete url_hbuf;
						delete name_hbuf;
						delete sinaID_hbuf;
						break;
					}
					
					iFondoGlobal->lrc_listAppend(url,sinaID,KNullDesC,name,singer);
					delete singer_hbuf;
					delete url_hbuf;
					delete name_hbuf;
					delete sinaID_hbuf;
				}
			}

						
		}
	}
//	
	delete sXml;
	iFondoGlobal->HideWaitNote();
	
	if(iFondoGlobal->m_lrc_count)
	{
		iFondoGlobal->m_appUI->view1->SetLrcLibleShowId(ELrcLibShow_HaveLRC); //ELrcLibShow_HaveLRC
		iFondoGlobal->m_appUI->view1->ChangeShowMode(2);
	}
	else
	{
		iFondoGlobal->m_appUI->view1->SetLrcLibleShowId(ELrcLibShow_NoAutoLRC); //ELrcLibShow_NoAutoLRC
	}
	

	FC_Log("CSinaInterfaceMgr::ParsePlayingInfo: --------OK----");
	return ret;
}
*/
void CSinaInterfaceMgr::HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3)
{
	FC_Log( "CSinaInterfaceMgr::HandleSessionEventL\r\n" );
	switch (aEvent)
	{
	case EMsvServerReady:
		// Initialise iMsvEntry
		if (!iMsvEntry)
		{
			iMsvEntry = CMsvEntry::NewL(*iMsvSession, KMsvGlobalInBoxIndexEntryId, TMsvSelectionOrdering());
		}
		if( !iClientMtmReg )
		{
			// Client-side MTM registry.
			iClientMtmReg = CClientMtmRegistry::NewL(*iMsvSession);

			// Get the SMS Mtm client from the registry
			iSmsMtm = static_cast<CSmsClientMtm*>(iClientMtmReg->NewMtmL(KUidMsgTypeSMS));
		}
		break;

	case EMsvEntriesCreated:
		// Only look for changes in the Inbox
		if (*(static_cast<TMsvId*>(aArg2)) == KObservedFolderId)
		{
			CMsvEntrySelection* entries = static_cast<CMsvEntrySelection*>(aArg1);
			iNewMessageId = entries->At(0);
		}
		break;

	case EMsvEntriesChanged:
		// Only look for changes in the Inbox
		if( !iObservingSms )
			break;


		if (*(static_cast<TMsvId*>(aArg2)) == KObservedFolderId)
		{
			CMsvEntrySelection* entries = static_cast<CMsvEntrySelection*>(aArg1);
			if (iNewMessageId == entries->At(0))
			{
				// It's the same message we received the EMsvEntriesCreated event for
				// Get from address (phone number)
				TBuf<32> fromAddress;
				iSmsMtm->SwitchCurrentEntryL( iNewMessageId );
				iSmsMtm->LoadMessageL();

				CSmsHeader& header = iSmsMtm->SmsHeader();
				fromAddress.Copy( header.FromAddress() );
				//CFondoGlobal::ShowMessage( fromAddress );
				if( 0!=fromAddress.Compare( iObservingNumber ) )
					break;

				// Set entry context to the new message
				iMsvEntry->SetEntryL(iNewMessageId);
				TMsvEntry entry=iMsvEntry->Entry();
				if( entry.iMtm!=KUidMsgTypeSMS || !entry.Complete())// Message is not complete
					return;
				if(entry.Unread())// unread
				{
					entry.SetInPreparation(EFalse);
					entry.SetNew(EFalse);
					entry.SetUnread(EFalse);		// set read
					iMsvEntry->ChangeL(entry);
				}
				else
					return;// the readed message

				// Open the store, read-only
				CMsvStore* store = iMsvEntry->ReadStoreL();
				CleanupStack::PushL(store);


				// Get body text and send it to the container
				if (store->HasBodyTextL())
				{
				}
				else
				{	
					iObservingContent.SetLength(0);
				}
				CleanupStack::PopAndDestroy(store);

				McgnOnComplete( KErrNone );

			}
		}
		break;

	default:
		break;
	}
}

void CSinaInterfaceMgr::McgnAfterUserSelectIAP(TInt aErr, TInt aIap)
{

}
//////////////////////////////////////////////////////////////////////////

TInt CSinaInterfaceMgr::McgnOnComplete(TInt aErr)
{
	TBuf<512> tempText;
	TBuf8<64> tempText8;

	if( aErr!=KErrNone )
	{
		FC_Logsp( "CSinaInterfaceMgr::McgnOnComplete error: %d,iTaskType=%d,iDownTask=%d",(int)aErr,(int)iTaskType,(int)iDownTask );
	}
	else
	{
		FC_Logsp("CSinaInterfaceMgr::McgnOnComplete:iTaskType=%d ,iDownTask=%d",(int)iTaskType,(int)iDownTask );
	}

	switch( iDownTask )
	{
	case 1001:			//get current playing lrc's url Auto
		{
			FC_LogT("McgnOnComplete download filename=%s",idownloadDes);
			iCenterCore->Download_ListDownloadCheck();
			TParse p;
			p.Set(idownloadDes,NULL,NULL);
			iDownTask = 0;
			if(iDownLoadGetter)
				delete iDownLoadGetter;
			iDownLoadGetter=NULL;

			if(aErr!=KErrNone )
				iCenterCore->Download_ListSetStatus(iCenterCore->m_curDownId,EdownloadErr);
			else
			{
				iCenterCore->Download_ListSetStatus(iCenterCore->m_curDownId,EdownloadOk);
				TInt type=iCenterCore->Download_ListGetType(iCenterCore->m_curDownId);
				FC_Logsp("Download_ListGetType type=%d,%d",type,iCenterCore->m_curDownId);
				if(type==MYLEFG_API_TYPE_MUSIC)
					iCenterCore->Music_localListAppend(idownloadDes, p.NameAndExt());
				else if(type==MYLEFG_API_TYPE_TXT)
					iCenterCore->Read_localListAppend(idownloadDes, p.NameAndExt());
				else if(type==MYLEFG_API_TYPE_LRC)
				{
					if(iCenterCore->m_musicStatus)
					{
						TBuf<32> name;
						music_list * templist=iCenterCore->m_musicMasklist->At(iCenterCore->m_iCurretPlayIndex);
						name.Copy(templist->title);
						name.SetLength(name.Length()-4);
						FC_LogT("McgnOnComplete p.Name()=%s",(TDes& )p.Name());
						FC_LogT("McgnOnComplete name=%s",name);
						
						if(name==p.Name())
						{
							FC_Logsp("McgnOnComplete   title.FindF");
							if(iCenterCore->m_appUI->viewMusic->iContainer)
							{	
								iCenterCore->m_appUI->viewMusic->iContainer->TimerStop();
								iCenterCore->Music_SetSubtitle(idownloadDes);
								iCenterCore->m_appUI->viewMusic->iContainer->UpdataAll();
							}
							else
							{
								iCenterCore->Music_SetSubtitle(idownloadDes);
							}

						}
					}
				}
			}
			iCenterCore->m_curDownId=-1;
			idownloadDes.SetLength(0);

			if(iCenterCore->m_appUI->viewDownLoad->iContainer)
				iCenterCore->m_appUI->viewDownLoad->iContainer->UpdataL();

			iCenterCore->ReSetTaskInfo();
			if(aErr==KErrNone )
				iCenterCore->Download_ListDownloadNext();
			FC_Logsp("McgnOnComplete::1001 download aErr=%d",aErr);
			return 1;
		}
		break;
	default :
		break;
	}
	
	switch( iTaskType )
	{
	case 0001:
		iTaskType = 0;
		ParseReadBangdanClass((TDes8&)iMusicGetter->GetMusicContent());
		break;
	case 0002:
	case 0021:
		ParseGetList((TDes8&)iMusicGetter->GetMusicContent());
		iTaskType = 0;
		break;
	case 0003:
	case 0004:
	case 0005:
	case 0006:
		ParseReadSearch((TDes8&)iMusicGetter->GetMusicContent());
		iTaskType = 0;			
		break;
	case 8:			//get current playing lrc's url Auto
		if( aErr==KErrNone )
		{
			ParseSearchLrc( (TDes8&)iMusicGetter->GetMusicContent() ) ;
		}
		iTaskType = 0;
		break;
	case 1001:			//get current playing lrc's url Auto
		iTaskType = 0;
		if(iDownLoadGetter)
			delete iDownLoadGetter;
		iDownLoadGetter=NULL;
		idownloadDes.SetLength(0);
		break;
	case 12:			//get current playing lrc's url Auto
		iTaskType = 0;
		break;
	case 7:  //get  current playing lrc's url Manual
		iTaskType = 0;
		break;
	case 41:
	case 42:
		iTaskType = 0;
		break;
	case 5555:
		iCenterCore->HideWaitNote();
		iTaskType = 0;
		break;
	default :
		break;
	}

	FC_Log( "CSinaInterfaceMgr::McgnOnComplete() OK");
return 1;
	
}

TInt CSinaInterfaceMgr::MmiftoAfterFetchImage(const TDesC& aId, void* aData, const TDesC& aFileName)
{
#ifdef _DEL_TEMPLETE_
	TInt i;
	CPlayList* playList = (CPlayList*)aData;

//	for( i=0; i<iFondoGlobal->GetPlstManager()->GetPlstCount(); i++ )
//	{
//		if( playList==iFondoGlobal->GetPlstManager()->GetPlayList(i) )
//		{
			//playList->ChangeAlbumPhotoAddress( aFileName );

			iFondoGlobal->iLogger->Log( _L("MmiftoAfterFetchImage: album photo updated: ") );
			iFondoGlobal->iLogger->Log( aFileName );
			iFondoGlobal->iLogger->Log( _L("\r\n") );

			iFondoGlobal->GetPlayListView()->AlbumPhotoUpdated( playList, aFileName );
			iFondoGlobal->GetNetSearchView()->TopUserPhotoUpdated( (TTopUserItem*)aData, aFileName );

			// update album image
			UpdateAlbumPhotoL();

			if( aData==NULL )
			{
				iAdInfo.iImage.Copy( aFileName );
				// Load ad image
				LoadAdImageL();
			}

			iFondoGlobal->iLogger->Log( _L("MmiftoAfterFetchImage: album photo updated ok\r\n") );
			
//		}
//	}
#endif
	return 0;
}

//////////////////////////////////////////////////////////////////////////
void CSinaInterfaceMgr::StartObserveSms(const TDesC& aObservNumber)
{
	FC_Log( "CSinaInterfaceMgr::StartObserveSms" );
	iObservingNumber.Copy( aObservNumber );
	iObservingSms = ETrue;
	if( !iMsvSession )
		iMsvSession = CMsvSession::OpenAsyncL(*this);
}

void CSinaInterfaceMgr::StopObserveSms()
{
	iObservingNumber.SetLength( 0 );
	iObservingSms = EFalse;	
}

void CSinaInterfaceMgr::StartTimer(TInt aMSecond)
{
	if( !iTaskTimer )
		iTaskTimer = CTimeOutTimer::NewL( CActive::EPriorityStandard, *this );
	
	if( iTaskTimer->IsActive() )
		iTaskTimer->Cancel();

	iTaskTimer->After( aMSecond*1000 );
}
void CSinaInterfaceMgr::StopTimer()
{
	if( iTaskTimer && iTaskTimer->IsActive() )
		iTaskTimer->Cancel();
}
void CSinaInterfaceMgr::ReleaseTimer()
{
	StopTimer();
	delete iTaskTimer;
}
void CSinaInterfaceMgr::TimerExpired ()
{
	FC_Log( "CSinaInterfaceMgr::TimerExpired.\r\n");
	switch( iTimerTaskType )
	{
	case 4:	// wait for register
		{
			TBuf<23> password;
			TBuf<256> msg;
			StopObserveSms();
			iTaskType = 0;

			password.Copy( iRegisterPassword );
			msg.Format( iCenterCore->TextResource( R_TEXT_PLAYBACK_DIALOG_REGISTER_DONE ), &password );
		}
		break;
	default:
			break;
	}
}
