/*
============================================================================
Name        : MusicImagesFetchTask.h
Author      : Wo Junjun
Version     : 2008-04-03
Copyright   : Funinhand
Description : CMusicImagesFetchTask declaration
============================================================================
*/

#include "MusicContentGetter.h"

struct TMusicImageItem
{
	TBuf<32> iImageId;
	void *iData;
	TFileName iFileName;
	TBuf<256> iUrl;
};

class MMusicImagesFetchTaskObserver;

class CMusicImagesFetchTask : public CBase, public MMusicContentGetterNotifier
{
public:
	static CMusicImagesFetchTask* NewL(MMusicImagesFetchTaskObserver* aObserver);
	~CMusicImagesFetchTask();

protected:
	void ConstructL(MMusicImagesFetchTaskObserver* aObserver);

	//
	/*
	From MMusicContentGetterNotifier
	*/
	TInt McgnOnComplete(TInt aErr);
	void McgnAfterUserSelectIAP(TInt aErr, TInt aIap) ;
protected:
	MMusicImagesFetchTaskObserver *iObserver;

	CMusicContentGetter* iImageGetter;
	CArrayFixFlat<TMusicImageItem*> *iTaskList;
	TBool iFetchingImage;

	void StartFetchImage();
	void RemoveFirstTaskItem();

public:
	void AddTask( const TDesC& aImageId, const TDesC& aUrl, const TDesC& aFileName, void* aData );
	void CancelAllTask();
};


class MMusicImagesFetchTaskObserver
{
public:
	virtual TInt MmiftoAfterFetchImage(const TDesC& aId, void* aData, const TDesC& aFileName) = 0;
};