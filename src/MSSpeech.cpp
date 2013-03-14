#include "MSSpeech.h"
#include "comutil.h"


typedef struct{
	TSWebProxyObject* adapter;
} Params;

QString result;
CSpDynamicString dstrText;
CComPtr<ISpRecoResult> RecoResult;

TSWebProxyObject::TSWebProxyObject(QObject *parent) :
QObject(parent)
{
	loadAddressbook();
	isDic=false;
	system_state=WAIT_DESTINATION;
	m_Window=static_cast<QWidget*>(parent);
	init(m_Window->winId());
}

void TSWebProxyObject::loadAddressbook(){
	addressbook.insert(std::pair<ULONG,QString>(1, "3941 O'Hara Street, Pittsburgh, PA 15260"));
	addressbook.insert(std::pair<ULONG,QString>(2, "159 Riverview Avenue, Pittsburgh, PA 15214"));
	addressbook.insert(std::pair<ULONG,QString>(3, "4227 Fifth Avenue, Pittsburgh, PA 15260"));
	addressbook.insert(std::pair<ULONG,QString>(4, "College Drive, Oak Hills, Butler, PA 16003"));
	addressbook.insert(std::pair<ULONG,QString>(5, "315 South Bellefield Avenue, Pittsburgh, PA 15213"));
	addressbook.insert(std::pair<ULONG,QString>(6, "3700 O'Hara Street, Pittsburgh, PA 15261"));
	addressbook.insert(std::pair<ULONG,QString>(7, "200 Lothrop Street,	Pittsburgh, PA 15213"));
	addressbook.insert(std::pair<ULONG,QString>(8, "3705 Fifth Avenue, Pittsburgh, PA 15213"));
	addressbook.insert(std::pair<ULONG,QString>(9, "219 Parkman Avenue, Pittsburgh, PA 15260"));
	addressbook.insert(std::pair<ULONG,QString>(10, "4200 Fifth Avenue,	Pittsburgh, PA 15260"));
	addressbook.insert(std::pair<ULONG,QString>(11, "Fifth & Ruskin Avenues, Pittsburgh, PA 15260"));
	addressbook.insert(std::pair<ULONG,QString>(12, "5150 Centre Avenue, Pittsburgh, PA 15232"));
	addressbook.insert(std::pair<ULONG,QString>(13, "Robinson Street, Pittsburgh, PA 15261"));
	addressbook.insert(std::pair<ULONG,QString>(14, "Fifth & Ruskin Avenues, Pittsburgh, PA 15260"));
	addressbook.insert(std::pair<ULONG,QString>(15, "University Drive, Pittsburgh, PA 15260"));
	addressbook.insert(std::pair<ULONG,QString>(16, "3943 O'Hara Street, Pittsburgh, PA 15260"));
	addressbook.insert(std::pair<ULONG,QString>(17, "3700 O'Hara Street, Pittsburgh, PA 15260"));
	addressbook.insert(std::pair<ULONG,QString>(18, "University Drive, Pittsburgh, PA 15261"));
	addressbook.insert(std::pair<ULONG,QString>(19, "477 Melwood Avenue, Pittsburgh, PA 15213"));
	addressbook.insert(std::pair<ULONG,QString>(20, "Schenley Drive, Pittsburgh, PA 15260"));
	addressbook.insert(std::pair<ULONG,QString>(21, "Atwood & Sennott Streets, Pittsburgh, PA 15260"));
	addressbook.insert(std::pair<ULONG,QString>(22, "Thackeray & O'Hara Streets, Pittsburgh, PA 15260"));
	addressbook.insert(std::pair<ULONG,QString>(23, "135 North Bellefield Avenue, Pittsburgh, PA 15213"));
	addressbook.insert(std::pair<ULONG,QString>(24, "Fifth & Ruskin Avenues, Pittsburgh, PA 15260"));
	addressbook.insert(std::pair<ULONG,QString>(25, "3942 Forbes Avenue, Pittsburgh, PA 15260"));
	addressbook.insert(std::pair<ULONG,QString>(26, "3939 O'Hara Street, Pittsburgh, PA 15260"));
	addressbook.insert(std::pair<ULONG,QString>(27, "3708 Fifth Avenue, Pittsburgh, PA 15213"));
	addressbook.insert(std::pair<ULONG,QString>(28, "4400 Fifth Avenue, Pittsburgh, PA 15213"));
	addressbook.insert(std::pair<ULONG,QString>(29, "Roberto Clemente Drive	Pittsburgh, PA 15260"));
	addressbook.insert(std::pair<ULONG,QString>(30, "7 Horsman Drive and Cochran Road, Pittsburgh, PA 15228"));
	addressbook.insert(std::pair<ULONG,QString>(31, "4337 Fifth Avenue, Pittsburgh, PA 15260"));
	addressbook.insert(std::pair<ULONG,QString>(32, "3943 O'Hara Street, Pittsburgh, PA 15260"));
	addressbook.insert(std::pair<ULONG,QString>(33, "Penn Center East, 400 Penn Center Blvd, Pittsburgh, PA 15235"));
	addressbook.insert(std::pair<ULONG,QString>(34, "3719 Terrace Street, Pittsburgh, PA 15261"));
	addressbook.insert(std::pair<ULONG,QString>(35, "130 DeSoto Street, Pittsburgh, PA 15261"));
	addressbook.insert(std::pair<ULONG,QString>(36, "13142 Hartstown Road,  Linesville, PA 16424"));
	addressbook.insert(std::pair<ULONG,QString>(37, "3460 Fifth Avenue, Pittsburgh, PA 15213"));
	addressbook.insert(std::pair<ULONG,QString>(38, "210 S. Bouquet Street, Pittsburgh, PA 15213"));
	addressbook.insert(std::pair<ULONG,QString>(39, "4107 O'Hara Street, Pittsburgh, PA 15260"));
	addressbook.insert(std::pair<ULONG,QString>(40, "139 University Place, Pittsburgh, PA 15260"));
	addressbook.insert(std::pair<ULONG,QString>(41, "3943 O'Hara Street, Pittsburgh, PA 15260"));
	addressbook.insert(std::pair<ULONG,QString>(42, "Allequippa & Darragh Streets, Pittsburgh, PA 15261"));
	addressbook.insert(std::pair<ULONG,QString>(43, "200 Meyran Avenue, Pittsburgh, PA 15260"));
	addressbook.insert(std::pair<ULONG,QString>(44, "3500 Victoria Street, Pittsburgh, PA 15261"));
	addressbook.insert(std::pair<ULONG,QString>(45, "230 S. Bouquet Street, Pittsburgh, PA 15260"));
}

void TSWebProxyObject::ExecuteCommand( const ULONG ulRuleID, const ULONG ulVal, const QString& command/* = QString("")*/ ){
	switch(ulRuleID){
	case 1:
		switch(ulVal){
		case 1:
			if(system_state==WAIT_DESTINATION||system_state==WAIT_SOURCE){
				speak("Please tell me the building name of your destination");
                system_state=WAIT_DESTINATION;
			}
			break;
		case 2:
			if(system_state==WAIT_SOURCE||system_state==WAIT_GET_PATH){
				speak("Please tell me the building name of your origin");
				system_state=WAIT_SOURCE;
			}
			break;
		}
		break;
	case 2:
		switch(ulVal){
		case 1:
			if(system_state==WAIT_GET_PATH||system_state==WAIT_START_ROUTE){
			    emit GetPath();
				system_state=WAIT_START_ROUTE;
			}
			break;
		case 2:
			if(system_state==WAIT_START_ROUTE){
				emit RouteStart();
			}
			break;
		case 3:
			speak("Thank you for using our system");
			emit RouteStop();
			system_state=WAIT_DESTINATION;
			break;
		}
		break;
	case 3:
		std::map<ULONG, QString>::iterator iter;
		iter = addressbook.find(ulVal);
		if(iter!=addressbook.end()){
			phraseCommand(command, iter->second);
		}
		break;
	}
}


void listenProcess(LPARAM lpParam){
	static const WCHAR wszUnrecognized[] = L"<Unrecognized>";
	TSWebProxyObject* adapter=((Params*)lpParam)->adapter;
	CSpEvent event; 
	SPPHRASE *pElements;
	HRESULT	hr;
	char *p;

	while(1){
	while ( event.GetFrom(adapter->cpRecoContext) == S_OK )
	{
		
		switch (event.eEventId)
		{
		case SPEI_RECOGNITION: 
			//OnRecoSuccess ( event.RecoResult() );
			// store the recognition result pointer
			RecoResult =event.RecoResult();

			dstrText.Clear();
			
			hr = RecoResult->GetText(SP_GETWHOLEPHRASE, SP_GETWHOLEPHRASE, TRUE,&dstrText, NULL);
			// release recognition result pointer in event object
			BSTR SRout;
			dstrText.CopyToBSTR(&SRout);
			dstrText.Clear();
			result.clear();
			p = _com_util::ConvertBSTRToString(SRout);
			result = QString(p);

			if (adapter->isDic)
			{
				if(SUCCEEDED(hr)){
					adapter->phraseCommand(result);
					//adapter->callbackFunc(result);
					event.Clear();
				}
			}else{
				if (SUCCEEDED(RecoResult->GetPhrase(&pElements)))
				{
					RecoResult->GetText(SP_GETWHOLEPHRASE, SP_GETWHOLEPHRASE, TRUE,&dstrText, NULL);

					adapter->ExecuteCommand( pElements->pProperties->ulId,
						pElements->pProperties->vValue.ulVal, result );
					//释放我们分配的pElements内存空间
					::CoTaskMemFree(pElements);
				}
			}
			break;
		case SPEI_FALSE_RECOGNITION:
			//OnRecoFail ();
			dstrText=wszUnrecognized;
			break;
		case SPEI_START_SR_STREAM:
			//OnStreamStart ();
			dstrText=wszUnrecognized;
			break;
		case SPEI_END_SR_STREAM:
			//OnStreamEnd ();
			dstrText=wszUnrecognized;
			break;
		}

	}
	}

}

void TSWebProxyObject::phraseCommand(const QString& command, const QString& value /*= QString("")*/){
	if(system_state==WAIT_DESTINATION){
		emit SetDestination(command, value);
		system_state=WAIT_SOURCE;
		return;
	}
	if(system_state==WAIT_SOURCE){
		emit SetSource(command, value);
		system_state=WAIT_GET_PATH;
		return;
	}
}

void TSWebProxyObject::init(HWND dlg){
	m_hWnd=dlg;
	CoInitialize(NULL);     
	if (FAILED(CoCreateInstance(CLSID_SpVoice, NULL,    CLSCTX_INPROC_SERVER, IID_ISpVoice, (void **)&pSpVoice)))  
	{   return;}
	HRESULT hr = g_cpEngine.CoCreateInstance(CLSID_SpSharedRecognizer);
	if (hr)
	{
		return;
	}
	hr = g_cpEngine->CreateRecoContext(&cpRecoContext);
	if(hr){
		return;
	}
	hr =  cpRecoContext->SetNotifyWindowMessage(m_hWnd, WM_RECOEVENT, 0, 0);
	if(hr){
		return;
	}
	const ULONGLONG ullInterest = SPFEI(SPEI_SOUND_START) | SPFEI(SPEI_SOUND_END) |
		SPFEI(SPEI_RECOGNITION) ;
	hr = cpRecoContext->SetInterest(ullInterest, ullInterest);
	if(hr){
		return;
	}
	hr = cpRecoContext->CreateGrammar(GID_CMD_GR, &cpRecoGrammar);
	if (hr)
	{
		return;
	}
	hr = cpRecoGrammar->LoadCmdFromFile (L"speech.xml", SPLO_DYNAMIC );
	if (hr)
	{
		int n=1;
		return;
	}
	hr=cpRecoContext->CreateGrammar(GID_DICTATION,&cpDicGrammar);
	if (hr)
	{
		return;
	}
	hr=cpDicGrammar->LoadDictation(NULL, SPLO_STATIC);
	if (hr)
	{
		return;
	}
}

void TSWebProxyObject::speak(QString content){
	pauseListening();
	if(!pSpVoice){
		return;
	}
	// required size
	WCHAR* str = new WCHAR[content.length() + 1];
	content.toWCharArray(str);
	str[content.length()] = _T('\0');
	pSpVoice->Speak(str, SPF_DEFAULT, NULL); 
    resumeListening();

	delete[] str;
}

//set dictionary state to Active and start a process to deal with the recognize events
void TSWebProxyObject::startListening(){
	HRESULT hr;
	//Active all rules by setting the first arg NULL
	hr = cpRecoGrammar->SetRuleState ( NULL, NULL, SPRS_ACTIVE );//not for dictionary
	if (hr)
	{
		return;
	}
	/*hr = cpRecoGrammar->SetDictationState(SPRS_ACTIVE);
	if (hr)
	{
		return;
	}*/
	HANDLE hThread;
	DWORD dwThreadId;
	Params* args=new Params();
	args->adapter = this;
	hThread = CreateThread(
		NULL,    
		NULL,   
		(LPTHREAD_START_ROUTINE)listenProcess,   
		args,    
		0,    
		&dwThreadId   
		);
	/**/

}

void TSWebProxyObject::switchToDic(){
	HRESULT hr;
	hr = cpRecoGrammar->SetRuleState ( NULL, NULL, SPRS_INACTIVE );//not for dictionary
	if (hr)
	{
		return;
	}
	hr = cpDicGrammar->SetDictationState(SPRS_ACTIVE);
	if (hr)
	{
		return;
	}
	isDic=true;
}

void TSWebProxyObject::switchToReco(){
	HRESULT hr;
	hr = cpRecoGrammar->SetRuleState ( NULL, NULL, SPRS_ACTIVE );//not for dictionary
	if (hr)
	{
		return;
	}
	hr = cpDicGrammar->SetDictationState(SPRS_INACTIVE);
	if (hr)
	{
		return;
	}
	isDic=false;
}

void TSWebProxyObject::resumeListening(){
	HRESULT hr;
	hr = cpRecoGrammar->SetRuleState ( NULL, NULL, SPRS_ACTIVE );//not for dictionary
	if (hr)
	{
		return;
	}
}

void TSWebProxyObject::pauseListening(){
	HRESULT hr;
	hr = cpRecoGrammar->SetRuleState ( NULL, NULL, SPRS_ACTIVE );//not for dictionary
	if (hr)
	{
		return;
	}
}

void TSWebProxyObject::endListening(){
	HRESULT hr;
	hr = cpRecoGrammar->SetDictationState(SPRS_INACTIVE);
	if (hr)
	{
		return;
	}
	hr = cpRecoGrammar->UnloadDictation();
	if (hr)
	{
		return;
	}

}

