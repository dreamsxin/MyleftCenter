/*
============================================================================
Name        : MusicImagesFetchTask.cpp
Author      : Wo Junjun
Version     : 2008-04-03
Copyright   : Funinhand
Description : CMusicImagesFetchTask implementation
============================================================================
*/

#include "MusicImagesFetchTask.h"
#include "const.h"
#include "CenterCore.h"

CMusicImagesFetchTask* CMusicImagesFetchTask::NewL(MMusicImagesFetchTaskObserver* aObserver)
{
	CMusicImagesFetchTask* self = new (ELeave) CMusicImagesFetchTask();
	CleanupStack::PushL( self );
	self->ConstructL( aObserver );
	CleanupStack::Pop();

	return self;
}

CMusicImagesFetchTask::~CMusicImagesFetchTask()
{
	CancelAllTask();
	delete iTaskList;

	delete iImageGetter;
}

void CMusicImagesFetchTask::ConstructL(MMusicImagesFetchTaskObserver* aObserver)
{
	iTaskList = new (ELeave) CArrayFixFlat<TMusicImageItem*> (KPlstArrayGranularity);	
	iObserver = aObserver;
}

void CMusicImagesFetchTask::AddTask( const TDesC& aImageId, const TDesC& aUrl, const TDesC& aFileName, void* aData )
{
	TMusicImageItem* item = new TMusicImageItem;
	item->iImageId.Copy( aImageId );
	item->iData = aData;
	item->iUrl.Copy( aUrl );
	item->iFileName.Copy( aFileName );
	iTaskList->AppendL( item );

	StartFetchImage();
}

void CMusicImagesFetchTask::CancelAllTask()
{
	if( iImageGetter )
		iImageGetter->CancelFetch();
	for( TInt i=0; i<iTaskList->Count(); i++ )
		delete iTaskList->At(i);
	iTaskList->Delete( 0, iTaskList->Count() );

	iFetchingImage = EFalse;
}

void CMusicImagesFetchTask::StartFetchImage()
{
	if( iTaskList->Count()<=0 || iFetchingImage )
		return;

	TMusicImageItem* item = iTaskList->At(0);
	if( !item )
		return;

	TInt err;
	if( !iImageGetter )
	{
		iImageGetter = CMusicContentGetter::NewL(CenterCore::GetInstance()->_http_iap_id);
	}

	err = iImageGetter->FetchAndSaveToFile( item->iUrl, item->iFileName, this );
	if( err!=KErrNone )
	{	// fetch fail
		RemoveFirstTaskItem();
		StartFetchImage();
		return;
	}

	iFetchingImage = ETrue;
}

void CMusicImagesFetchTask::RemoveFirstTaskItem()
{
	if( iTaskList->Count()>0 )
	{
		delete iTaskList->At(0);
		iTaskList->Delete(0, 1);
	}
}

TInt CMusicImagesFetchTask::McgnOnComplete(TInt aErr)
{
	if( aErr==KErrNone )
	{
		TMusicImageItem *item = iTaskList->At(0);
		if( item )
		{
			iObserver->MmiftoAfterFetchImage( item->iImageId, item->iData, item->iFileName );
		}
	}
	iFetchingImage = EFalse;
	RemoveFirstTaskItem();
	StartFetchImage();
	return 1;
}

void CMusicImagesFetchTask::McgnAfterUserSelectIAP(TInt aErr, TInt aIap) 
{}