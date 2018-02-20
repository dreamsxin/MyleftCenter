/*
============================================================================
 Name        : MdaAudioShell.cpp 
 Author      : yilizheng
 Version     :
 Copyright   : Your copyright notice
 Description : for play Media 
============================================================================
*/

#include <coemain.h>
#include <eikenv.h>
#include "MdaAudioShell.h"
#include "CenterCore.h"
#include "portab_symbian.h"
#include <e32std.h>
#include <mmf\common\mmfcontrollerframeworkbase.h>

#include  "MyleftCenterViewMusic.h"
#include "MyleftCenterContainerMusic.h"

_LIT(KRecorderFile, "c:\\funvv\\record.wav"); 

CMdaAudioShell* CMdaAudioShell::NewL (CenterCore * owner) 
{
	FC_Log("CMdaAudioShell::NewL ");
	
	CMdaAudioShell *self = new (ELeave) CMdaAudioShell();
	CleanupStack::PushL (self);
	self->ConstructL (owner);
	CleanupStack::Pop ();
	
	return self;
}
CMdaAudioShell::CMdaAudioShell() :CurTime(0),m_status(EMdaUninit)
{}

void CMdaAudioShell::ConstructL (CenterCore * owner) 
{
	iCenterCore=owner;
	iMdaAudio=CMdaAudioRecorderUtility::NewL(*this);
}

CMdaAudioShell::~CMdaAudioShell () 
{
	if(iMdaAudio)
		delete iMdaAudio;
}


void CMdaAudioShell::OpenMdeiaL(const TDesC& aFileName)
{
	FC_Log("CMdaAudioShell::OpenMdeiaL()");
	filname=aFileName;
	if(iMdaAudio)
	{
		iMdaAudio->Close();
		iMdaAudio->OpenFileL(aFileName);	
		m_status=EMdaReady;
	}
}

void CMdaAudioShell::Play()
{
	FC_Log("CMdaAudioShell::Play() iMdaAudio->State()=%d,CurTime=%d",iMdaAudio->State(),CurTime.Int64());
	if(filname.Length() && iMdaAudio)
	{
		if(iMdaAudio->State()==CMdaAudioClipUtility::ENotReady)// 0
		{
			iMdaAudio->OpenFileL(filname);	
		}
		else if(iMdaAudio->State()==CMdaAudioClipUtility::EOpen) // 1
		{
			iMdaAudio->SetPosition(CurTime);
			iMdaAudio->PlayL();
			iCenterCore->m_musicStatus=CCCB_STATUS_PLAY;
		}
	//	iCenterCore->m_musicStatus=CCCB_STATUS_PLAY;
	//	iCenterCore->ReSetTaskInfo();
	}
}


void CMdaAudioShell::RecordL()
{
	TFileName aFileName;
	aFileName.Append(KRecorderFile);

	CEikonEnv::Static()->FsSession().Replace(aFileName, aFileName);

	iMdaAudio->OpenFileL(aFileName);
	iMdaAudio->SetAudioDeviceMode(CMdaAudioRecorderUtility::ETelephonyOrLocal);
	iMdaAudio->SetVolume(iMdaAudio->MaxVolume());
	iMdaAudio->SetGain(iMdaAudio->MaxGain());
	iMdaAudio->SetPosition(TTimeIntervalMicroSeconds(0));
	iMdaAudio->CropL();
	iMdaAudio->RecordL();
} 

void CMdaAudioShell::ParseMetaDataValueL(CMMFMetaDataEntry &aMetaData, TDes &aDestinationBuffer )
{
	HBufC * tempBuf = aMetaData.Value().AllocL();
	CleanupStack::PushL(tempBuf);
	tempBuf->Des().Zero();
	TLex parse(aMetaData.Value() );
	TBool first=ETrue;
	for ( ; ; )
	{
		TPtrC16 token = parse.NextToken();
		if (token.Length() <=0 )
			break;
		if (!first)
			tempBuf->Des().Append(_L(" "));
		tempBuf->Des().Append(token); 
		first = EFalse;

	}

	// Crop to the final buffer
	aDestinationBuffer = tempBuf->Left(aDestinationBuffer.MaxLength());
	CleanupStack::PopAndDestroy(tempBuf);
}

void CMdaAudioShell::MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode)
{
	FC_Log("CMdaAudioShell::MoscoStateChangeEvent() aPreviousState=%d,aCurrentState=%d,aErrorCode=%d",(int)aPreviousState,(int)aCurrentState,(int)aErrorCode);	

	 if ( aErrorCode == KErrUnderflow ) 
	{
			StopL();
	}
	else if ( aErrorCode == KErrDied ||aErrorCode ==KErrAbort)  //phone
	{
		FC_Log("MaoscPlayComplete:KErrDied");
		PauserL();
	}
	else if ( aErrorCode == KErrInUse ) //sms
	{
		PauserL();
		FC_Log("MaoscPlayComplete:KErrInUse");
	}

	if(aErrorCode)
		return;
	
	switch (aCurrentState)
	{
		case CMdaAudioClipUtility::EOpen:  // 1
		{
			if(aPreviousState==CMdaAudioClipUtility::EPlaying)// aPreviousState =2,aCurrentState=1
			{
				if(m_status==EMdaPlaying)// play end
					m_status=EMdaStop;
				iMdaAudio->Stop();
				CurTime=TTimeIntervalMicroSeconds(0);
				iCenterCore->Music_PlayerListNext();
				if(iCenterCore->m_appUI->viewMusic->iContainer)
					iCenterCore->m_appUI->viewMusic->iContainer->UpdataAll();

			}
			else if(aPreviousState==CMdaAudioClipUtility::ENotReady) // aPreviousState =0,aCurrentState=1 
			{
				FC_Log("MaoscPlayComplete:ready to play");
				GetMMFInfo(iCenterCore->curPlayer->name,iCenterCore->curPlayer->singer,(TInt&)iCenterCore->m_duration);
				
				iMdaAudio ->SetAudioDeviceMode(CMdaAudioRecorderUtility::ELocal);
				iMdaAudio ->SetVolume(iCenterCore->m_musicSetting.volume);
				iMdaAudio->SetPosition(CurTime);
				iMdaAudio->PlayL();
				iCenterCore->m_musicStatus=CCCB_STATUS_PLAY;
				iCenterCore->ReSetTaskInfo();

		//	m_status=EMdaPlaying;
			}
		}
		break;
		case CMdaAudioClipUtility::EPlaying:    // aPreviousState=  , aCurrentState=2 
		{
			TInt volume;
			iMdaAudio->GetVolume(volume);
			FC_Log("CMdaAudioShell:: volume=%d",volume);
			m_status=EMdaPlaying;		
		}
		break;
		default://ENotReady
			break;
	}
}


void CMdaAudioShell::PauserL()
{
	if(iMdaAudio->State()==CMdaAudioClipUtility::EPlaying)
	{
		FC_Log("CMdaAudioShell::PauserL() ");
		CurTime=iMdaAudio->Position();   //befare stop
		iMdaAudio->Stop();
		m_status=EMdaPauser;
	}
	iCenterCore->m_musicStatus=CCCB_STATUS_PAUSE;

}


void CMdaAudioShell::StopL()
{
	FC_Log("CMdaAudioShell::StopL() ");
	if(iMdaAudio->State()==CMdaAudioClipUtility::EPlaying)
	{
		iMdaAudio->Stop();
	}
	CurTime=TTimeIntervalMicroSeconds(0);
	m_status=EMdaStop;

	iCenterCore->m_musicStatus=CCCB_STATUS_STOP;
}

void CMdaAudioShell::SetVolume(TInt _volume)
{
	if(iMdaAudio)
		iMdaAudio ->SetVolume(_volume);
}

TInt64 CMdaAudioShell::GetPosition()
{
	if(iMdaAudio)
	{
		if(iCenterCore->m_musicStatus==CCCB_STATUS_PAUSE)
			return CurTime.Int64();
		else
			return	iMdaAudio->Position().Int64();
	}
//	return TInt(posint.GetTInt());
}
void CMdaAudioShell::SetPosition(TInt _MicroSeconds)
{
	CurTime=TTimeIntervalMicroSeconds(_MicroSeconds);
}

void CMdaAudioShell::GetMMFInfo( TDes& _name,TDes& _signer,TInt & _MicroSeconds)
{
	TInt nbMetaData;
	CMMFMetaDataEntry * aMetaData;
	FC_Log("CMdaAudioShell::GetMMFInfo  setp 1");
	TInt err = iMdaAudio->GetNumberOfMetaDataEntries(nbMetaData);
	if (err != KErrNone)
		nbMetaData = 0;

	FC_Log("CMdaAudioShell::GetMMFInfo  setp 2,nbMetaData=%d,err=%d",nbMetaData,(int)err);
	HBufC *metadataValue = HBufC::NewLC(128);
	TPtr metadataValueDes(metadataValue->Des());
	for (TInt i=0; i< nbMetaData; i++)
	{
		aMetaData = iMdaAudio->GetMetaDataEntryL(i);
		ParseMetaDataValueL(*aMetaData, metadataValueDes);

		const TDesC& name(aMetaData->Name());
		const TDesC& value(aMetaData->Value());
		
		
		if ( name == _L("title") )
		{
			_name.Copy( *metadataValue );
		}
		if ( name == _L("artist"))
		{
			_signer.Copy( *metadataValue );
		}
		delete aMetaData;
	}        
	CleanupStack::PopAndDestroy(metadataValue);
	
	TTimeIntervalMicroSeconds AllTime=iMdaAudio->Duration();
#ifdef _S60_2RD_
	_MicroSeconds=AllTime.Int64().GetTInt()/1000000;
#else
	_MicroSeconds=(TInt)(AllTime.Int64()/1000000);
#endif	

}
