/*
============================================================================
 Name        : MdaAudioShell.h 
 Author      : yilizheng
 Version     :
 Copyright   : Your copyright notice
 Description : for play Media handle file
============================================================================
*/
#ifndef _MDA_AUDIO_SHELL_H_
#define _MDA_AUDIO_SHELL_H_

#include <mda\client\utility.h>
#include <MdaAudioSampleEditor.h>

class CenterCore;
class CMdaAudioShell: public CBase, public MMdaObjectStateChangeObserver
{
public:
	enum TMdaShellState {
		EMdaUninit,  //0
		EMdaReady, // 1
		EMdaPlaying,// 2
		EMdaPauser,// 3
		EMdaStop,// 4
		EMdaError //4// 5
		};
	
	CMdaAudioShell();
	~CMdaAudioShell();
static CMdaAudioShell *NewL(CenterCore * owner);
	void OpenMdeiaL(const TDesC& aFileName);
	void Play();
	void PauserL();
	void StopL();
	void RecordL();	
	void SetVolume(TInt _volume);
	TInt64 GetPosition();
	void SetPosition(TInt _MicroSeconds);
	void GetMMFInfo( TDes& _name,TDes& _signer,TInt & _MicroSeconds);

protected:
	void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);

private:
	void ConstructL(CenterCore * owner);
	void ParseMetaDataValueL(CMMFMetaDataEntry &aMetaData, TDes &aDestinationBuffer );

private:
	CenterCore * iCenterCore;
	CMdaAudioRecorderUtility *iMdaAudio;
	TTimeIntervalMicroSeconds CurTime;
	TBuf<256>  filname;

public:
	TMdaShellState	m_status;
}; 






#endif
