/*
============================================================================
 Name        : FunSMusicFileMgrContainer from FunSMusicFileMgrContainer.h
 Author      : yilizheng
 Version     :
 Copyright   : Your copyright notice
 Description : Container control PlayList
============================================================================
*/

// INCLUDE FILES
#include "MyleftCenterContainerFileMgr.h"

//#include <eiklabel.h>  // for example label control

#include <MyleftCenter.rsg>
#include "MyleftCenter.hrh"
#include <aknmessagequerydialog.h> 
#include <AknIconArray.h> 
#include <barsread.h>
#include <StringLoader.h> 
#include <eikfrlb.h>   //CFormattedCellListBoxItemDrawer()
#include <eikclbd.h>	//CColumnListBoxData::SetIconArray()
#include <f32file.h>
#include <MyleftCenter.mbg>
#include "Centercore.h"
#include "MyleftCenterViewFileMgr.h"
#include "const.h"
// ================= MEMBER FUNCTIONS =======================



	CMyleftCenterContainerFileMgr* CMyleftCenterContainerFileMgr::NewL(CMyleftCenterViewFileMgr *parent,const TRect& aRect)
	{
			CMyleftCenterContainerFileMgr* self=CMyleftCenterContainerFileMgr::NewLC(parent,aRect);
			CleanupStack::Pop(self);
			return self;

	}

	CMyleftCenterContainerFileMgr* CMyleftCenterContainerFileMgr::NewLC(CMyleftCenterViewFileMgr *parent,const TRect& aRect)
	{
			CMyleftCenterContainerFileMgr* self=new(ELeave) CMyleftCenterContainerFileMgr;
			CleanupStack::PushL(self);
			self->ConstructL(parent,aRect);
			return self;
	}

// ---------------------------------------------------------
// FunVVContainerList::ConstructL(const TRect& aRect)
// EPOC two phased constructor
// ---------------------------------------------------------
//
void CMyleftCenterContainerFileMgr::ConstructL(CMyleftCenterViewFileMgr *parent,const TRect& aRect)
    {
    CreateWindowL();

	iplaycore=CenterCore::GetInstance();
	if(!iplaycore)
		return ;

	iparent=parent;
/*	
	iMainBackImg=FunUiLib::CreateBitmapL(KMusicMultiBitmapFilename,EMbmFunsmusicMain_back);
	if(!iMainBackImg)
		return;
*/	
	TInt iconIds[] = {EMbmMyleftcenterFile, EMbmMyleftcenterFolder, EMbmMyleftcenterFolder_up};
	TInt maskIds[] = {EMbmMyleftcenterFile_mask, EMbmMyleftcenterFolder_mask, EMbmMyleftcenterFolder_up_mask};

	iFileMgrList= CFunList::NewL( aRect, this, FunUiLib::NormalFont());
	iFileMgrList->SetupGraphicIcons( KMusicMultiBitmapFilename, iconIds, maskIds, 3, 0, 13, 13);
	iFileMgrList->BackDrawer()->SetGradientBackground( CFunBackgroundDrawer::EVerticalOrientation,KRgbWhite, KRgbContainer);
	iFileMgrList->ActivedBackDrawer()->SetBackgroundColor( TRgb(248,227,133) );
	iFileMgrList->ActivedBackDrawer()->SetNoBorder();
	iFileMgrList->SetTextColors( TRgb(75,75,100), TRgb(1,1,1),  TRgb(1,1,1));
	//iFileMgrList->ChangeItemHeight(KDownload_ItemH);
	iFileMgrList->SetDisplayNumber(0);
	iFileMgrList->SetObserver(this);
	SetupListItemsL();
	
    SetRect(aRect);
    ActivateL();
    }

// Destructor
CMyleftCenterContainerFileMgr::~CMyleftCenterContainerFileMgr()
{
	if(iFileMgrList)
	{
		delete iFileMgrList;
	}
	
	if(iMainBackImg)
	{
		delete iMainBackImg;
	}
}

// ---------------------------------------------------------
// FunVVContainerList::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CMyleftCenterContainerFileMgr::SizeChanged()
    {
	if( iFileMgrList )
	{
	
		TRect listRect= Rect();
		listRect.Move( 1, 0 );
		listRect.Resize( -2, 0);
		
		iFileMgrList->SetExtent( listRect.iTl, listRect.Size() );
		
	}
	 DrawNow();
    }

// ---------------------------------------------------------
// CFunVVContainer::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CMyleftCenterContainerFileMgr::CountComponentControls() const
    {
    return 1; // return nbr of controls inside this container
    }

// ---------------------------------------------------------
// CFunVVContainer::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
//
CCoeControl* CMyleftCenterContainerFileMgr::ComponentControl(TInt aIndex) const
    {
    switch ( aIndex )
        {
        case 0:
            return iFileMgrList;
 
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------
// CFunVVContainer::Draw(const TRect& aRect) const
// ---------------------------------------------------------
//
void CMyleftCenterContainerFileMgr::Draw(const TRect& aRect) const
    {
    CWindowGc& gc = SystemGc();

	gc.SetPenStyle( CGraphicsContext::ESolidPen );
	gc.SetPenColor(KRgbRect);
	gc.DrawLine(TPoint(0,0),TPoint(0,aRect.Height()));
	gc.DrawLine(TPoint(aRect.Width()-1,0),TPoint(aRect.Width()-1,aRect.Height()));

    }

// ---------------------------------------------------------
// FunVVContainerList::HandleControlEventL(
//     CCoeControl* aControl,TCoeEvent aEventType)
// ---------------------------------------------------------
//

// ---------------------------------------------------------
// FunVVContainerList::OfferKeyEventL(
//     CCoeControl* aControl,TCoeEvent aEventType)
// ---------------------------------------------------------
//
TKeyResponse CMyleftCenterContainerFileMgr::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
    {
		//TInt code = aKeyEvent.iCode;

	if(aKeyEvent.iCode== EKeyBackspace)
	{
			if(!iFileMgrList->GetCurrentSelectIndex())
				return EKeyWasConsumed;
  			TInt ret=CFunMessageDialog::RunDlgLD(KNullDesC,iplaycore->TextResource(R_MESSAGE_DELETE),iplaycore->m_appUI->UiStyler(),NULL,CFunMessageDialog::EMessageDialog_OkCancel,CFunMessageDialog::EMessageIcon_Question);
			if(!ret)
				return EKeyWasConsumed;

			SaveCurPath(0);
			DoDelete();
			iFileMgrList->SetCurrentSelectIndex(0);
			UpdataL();
			return EKeyWasConsumed;
	}

	if ( iFileMgrList )
	{
		return iFileMgrList->OfferKeyEventL( aKeyEvent, aType );
	}
	return( EKeyWasNotConsumed );

    }

void CMyleftCenterContainerFileMgr::MfloListItemClicked(CFunList* aButton, TInt aIndex)
{
	DoClicked();
/*
	RFs iSessionRFs;
	CDir* dirList=NULL;
	FPlayerCore *iplaycore=FPlayerCore::GetInstance();
	
	if(fmPath.Length()==0)
	{
		if(aIndex==0)
			fmPath.Append(_L("c:\\"));
		else if(aIndex==1)
			fmPath.Append(_L("e:\\"));

		SetupListItemsL();
		iFileMgrList->SetCurrentSelectIndex(0);
		DrawNow();
	}
	else if(aIndex==0)   //back;
	{
			if( fmPath.Length()>3 )
			{
				fmPath.Delete(fmPath.Length()-1,1);
				int pos=fmPath.LocateReverse(TChar('\\'));
				fmPath.SetLength(pos+1);
			}else if( fmPath.Length()==3 )
			{
				fmPath.SetLength(0);
			}
			
			SetupListItemsL();
			iFileMgrList->SetCurrentSelectIndex(0);
			DrawNow();
	}
	else
	{
		User::LeaveIfError(iSessionRFs.Connect()); 
		User::LeaveIfError(iSessionRFs.GetDir(fmPath,KEntryAttMaskSupported,EDirsFirst,dirList));
		if((*dirList)[aIndex-1].IsDir())
		{
			fmPath.Append((*dirList)[aIndex-1].iName);
			fmPath.Append(_L("\\"));
			SetupListItemsL();
			iFileMgrList->SetCurrentSelectIndex(0);
			DrawNow();
		}
		else  //Open file
		{
			_LIT(KExtension3, ".mp3");
			TParse p;
			p.Set((*dirList)[aIndex-1].iName,NULL,NULL);
			TBuf<256> filename;
			filename.Append(fmPath);
			filename.Append((*dirList)[aIndex-1].iName);

			if(!p.Ext().CompareF(KExtension3))
			{
				song  templist;
				templist.iaskurl.Copy(filename);
				iplaycore->Music_playSong(&templist);
			}
			else
			{
				iplaycore->m_appUI->OpenOtherFileL(filename);
			}
		}
	}

	if(dirList)
		delete dirList;
	iSessionRFs.Close();
	*/
}


void CMyleftCenterContainerFileMgr::SetupListItemsL()
{
	_LIT(KCDrive, "CE");	
	_LIT (KString,"%S");
	_LIT (KString2,"%s (%S) [%d.%2dM/%d.%2dM]");

	TBuf<128> aString;
	TInt downCount=0;
	TInt downingCount=0;
	
	TDriveList aList;
	TVolumeInfo aVolume;
	RFs iSessionRFs;
	CDir* dirList=NULL;
	
	downCount=iFileMgrList->GetListItemCount();
	iFileMgrList->DeleteItems(0, downCount);

	if(iparent->fmPath.Length()==0)
	{
		iFileMgrList->SetIconMargin(0);

		User::LeaveIfError(iSessionRFs.Connect()); 
		User::LeaveIfError(iSessionRFs.DriveList(aList));
		TBuf<100> dirname;
		TChar cdir;

		for (TInt i = 0; i< KMaxDrives; i++)
		{	
			if(aList[i])
			{
				
				iSessionRFs.DriveToChar(i,cdir);
				if(cdir.GetUpperCase()==(TUint)KCDrive()[0])
				{
					cdir=TChar('C');
					User::LeaveIfError(iSessionRFs.Volume(aVolume,i));
					CEikonEnv::Static()->ReadResource (dirname, R_FILEMGR_PHONE);
				}
			#ifndef __WINS__
				else if(cdir.GetUpperCase()==(TUint)KCDrive()[1])
				{
					cdir=TChar('E');
		
					User::LeaveIfError(iSessionRFs.Volume(aVolume,i));
		
					CEikonEnv::Static()->ReadResource (dirname, R_FILEMGR_MEMORY);
				}
			#endif
				else
					continue;
				aString.Format(KString2(),&cdir,&dirname,aVolume.iFree/1048576,(aVolume.iFree%1048576)/1024,aVolume.iSize/1048576,(aVolume.iSize%1048576)/1024);
				iFileMgrList->AppendItemL( aString, -1, NULL);
			}
		}
	}
	else
	{
		iFileMgrList->SetIconMargin(8);
		
		User::LeaveIfError(iSessionRFs.Connect()); 
		User::LeaveIfError(iSessionRFs.GetDir(iparent->fmPath,KEntryAttMaskSupported,EDirsFirst,dirList));

		aString.Format(KString(),&iplaycore->TextResource(R_FILEMGR_UP));
		iFileMgrList->AppendItemL( aString, 2,NULL);

		for(int i=0;i<dirList->Count();i++)
		{
			if((*dirList)[i].IsDir())
			{
				aString.Format(KString(),&(*dirList)[i].iName);
				iFileMgrList->AppendItemL( aString,1,NULL);
		
			}
			else
			{
				aString.Format(KString(),&(*dirList)[i].iName);
				iFileMgrList->AppendItemL( aString,0,NULL);
			}
			
		}

	}

	iSessionRFs.Close();
	if(dirList)
		delete dirList;

}


 void CMyleftCenterContainerFileMgr::UpdataL()
 {
 	SetupListItemsL();
	DrawNow();
 }

 void CMyleftCenterContainerFileMgr::DoClicked()
 {
 	int aIndex=iFileMgrList->GetCurrentSelectIndex();
	RFs iSessionRFs;
	CDir* dirList=NULL;
	CenterCore *iplaycore=CenterCore::GetInstance();
	
	if(iparent->fmPath.Length()==0)
	{
		if(aIndex==0)
			iparent->fmPath.Append(_L("c:\\"));
		else if(aIndex==1)
			iparent->fmPath.Append(_L("e:\\"));

		SetupListItemsL();
		iFileMgrList->SetCurrentSelectIndex(0);
		DrawNow();
	}
	else if(aIndex==0)   //back;
	{
			if( iparent->fmPath.Length()>3 )
			{
				iparent->fmPath.Delete(iparent->fmPath.Length()-1,1);
				int pos=iparent->fmPath.LocateReverse(TChar('\\'));
				iparent->fmPath.SetLength(pos+1);
			}else if( iparent->fmPath.Length()==3 )
			{
				iparent->fmPath.SetLength(0);
			}
			
			SetupListItemsL();
			iFileMgrList->SetCurrentSelectIndex(0);
			DrawNow();
	}
	else
	{
		User::LeaveIfError(iSessionRFs.Connect()); 
		User::LeaveIfError(iSessionRFs.GetDir(iparent->fmPath,KEntryAttMaskSupported,EDirsFirst,dirList));
		if((*dirList)[aIndex-1].IsDir())
		{
			iparent->fmPath.Append((*dirList)[aIndex-1].iName);
			iparent->fmPath.Append(_L("\\"));
			SetupListItemsL();
			iFileMgrList->SetCurrentSelectIndex(0);
			DrawNow();
		}
		else  //Open file
		{
			_LIT(KExtension3, ".text");
			TParse p;
			p.Set((*dirList)[aIndex-1].iName,NULL,NULL);
			TBuf<256> filename;
			filename.Append(iparent->fmPath);
			filename.Append((*dirList)[aIndex-1].iName);

			if(!p.Ext().CompareF(KExtension3))
			{
			/*
				song  templist;
				int appendId;
				templist.iaskurl.Copy(filename);
				templist.name.Copy(p.NameAndExt());
				appendId=iplaycore->myAVAddItemToPlayer(templist.name, templist.iaskurl);
				iplaycore->playerSetCurIndex(appendId);
				iplaycore->Music_playSong(&templist);*/
			}
			else
			{
				iplaycore->m_appUI->OpenOtherFileL(filename);
			}
		}
	}

	if(dirList)
		delete dirList;
	iSessionRFs.Close();
}

  void CMyleftCenterContainerFileMgr::SaveCurPath(TInt _aim)
 {
 	int aIndex=iFileMgrList->GetCurrentSelectIndex();
	RFs iSessionRFs;
	CDir* dirList=NULL;
	CenterCore *iplaycore=CenterCore::GetInstance();

	if(!aIndex ||! iparent->fmPath.Length())
		return ;

	doPath.SetLength(0);
	doPath.Copy(iparent->fmPath);
	User::LeaveIfError(iSessionRFs.Connect()); 
	User::LeaveIfError(iSessionRFs.GetDir(iparent->fmPath,KEntryAttMaskSupported,EDirsFirst,dirList));
	if((*dirList)[aIndex-1].IsDir())
	{
		doPath.Append((*dirList)[aIndex-1].iName);
		doPath.Append(_L("\\"));
		is_doPathDir=ETrue;
	}
	else  //Open file
	{
		doPath.Append((*dirList)[aIndex-1].iName);
		is_doPathDir=EFalse;
	}

	if(dirList)
		delete dirList;
	iSessionRFs.Close();

	m_arm=_aim;
}

TInt CMyleftCenterContainerFileMgr::DoPlaster()
{
	TInt Error=0;
	TParse p;
	RFs fsSession;
	TBuf16<MAX_FILEMANE> fullName;
	User::LeaveIfError(fsSession.Connect()); 
	CFileMan* fileMan = CFileMan::NewL(fsSession);
	CleanupStack::PushL(fileMan); 

	fullName.Copy(iparent->fmPath);
	p.Set(doPath,NULL,NULL);
	if(is_doPathDir)
	{
		doPath.SetLength(doPath.Length()-1);
		p.Set(doPath,NULL,NULL);
		fullName.Append(p.Name());
		
	}
	else
	{
		fullName.Append(p.NameAndExt());
	}

	char path[256];
	memset(path,0,256);
	FC_TBuf16ToCharp(path, fullName);
	FC_TBuf16ToCharp(path, doPath);
	
	if(m_arm==1)
	{
		Error=fileMan->Move(doPath,fullName, CFileMan::EOverWrite); //KErrNone if successful
	}
	else if(m_arm==2)
	{
		if(is_doPathDir)
		{
			fullName.Append(_L("\\"));
			int error=fsSession.MkDirAll(fullName);
			if(!error)
				Error=fileMan->Copy(doPath,fullName, CFileMan::EOverWrite); //KErrNone if successful

		}
		else
		{
			Error=fileMan->Copy(doPath,fullName, CFileMan::EOverWrite); //KErrNone if successful
		}
	
	}
	CleanupStack::PopAndDestroy(); 
	fsSession.Close(); // close file server session

	doPath.SetLength(0);
	return Error;

}


TInt CMyleftCenterContainerFileMgr::DoDelete()
{
	TInt Error=0;
	RFs fsSession;		
	User::LeaveIfError(fsSession.Connect()); 

	if(is_doPathDir)
	{
		CFileMan* fileMan = CFileMan::NewL(fsSession);
		CleanupStack::PushL(fileMan); 
		
		Error=fileMan->RmDir(doPath); //KErrNone if successful		
		CleanupStack::PopAndDestroy(); 
	}
	else
	{
		Error=fsSession.Delete(doPath);
		
	}
	
	fsSession.Close();
	doPath.SetLength(0);
	return Error;
}

TInt CMyleftCenterContainerFileMgr::DoRename(TDesC & _newName)
{
	TInt Error=0;
	RFs fsSession;	
	TBuf16<MAX_FILEMANE> fullName;
	User::LeaveIfError(fsSession.Connect()); 

	fullName.Copy(iparent->fmPath);
	fullName.Append(_newName);
	Error=fsSession.Rename(doPath,fullName);
	
	fsSession.Close();
	doPath.SetLength(0);
	return Error;
}

TInt CMyleftCenterContainerFileMgr::DoCreatFolder(TDesC & _newName)
{
	int Error=0;
	TBuf16<MAX_FILEMANE> szDirectorybuf;
	RFs fsSession;
	
	User::LeaveIfError(fsSession.Connect()); 

	szDirectorybuf.Copy(iparent->fmPath);
	szDirectorybuf.Append(_newName);
	if(!(szDirectorybuf[szDirectorybuf.Length()-1]=='\\'))
		szDirectorybuf.Append(_L("\\"));
	Error=fsSession.MkDirAll(szDirectorybuf);//KErrNone if successful
	
	fsSession.Close(); // close file server session

	return Error;
}
