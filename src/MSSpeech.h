#ifndef _MS_SPEECH_DX
#define _MS_SPEECH_DX

#include "TSWebApp.h"

#include <sapi.h>
#include <sphelper.h>
#include <spuihelp.h>
#include <iostream>
#include <QWidget>
#include <QString>
#include <QObject>
#include <QMap>
#include <QStringList>
#include <QVariant>
#include <map>

#pragma comment(lib,"ole32.lib")   //CoInitialize CoCreateInstance需要调用ole32.dll
#pragma comment(lib,"sapi.lib")    
#pragma comment(lib,"comsupp.lib")  

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

#define GID_DICTATION   0           // Dictation grammar has grammar ID 0
#define GID_CMD_GR      33333
#define WM_RECOEVENT    WM_USER+1      // Window message used for recognition events

#define START_ROUTE_CMD "start route"
#define SET_DESTINATION_CMD "set destination"
#define SET_SOURCE_CMD "set source"
#define STOP_ROUTE_CMD "stop route"
#define GET_PATH_CMD "get path"
#define WAIT_DESTINATION 0
#define WAIT_SOURCE 1
#define WAIT_GET_PATH 2
#define WAIT_START_ROUTE 3
#define WAIT_STOP_ROUTE 4


class TSWebProxyObject:public QObject{
	Q_OBJECT
public:
	explicit TSWebProxyObject(QObject *parent = 0);
	virtual ~TSWebProxyObject(){};
	void init(HWND dlg);//set callback function for receiving what the machine has heard & init the MSSpeach

	HWND m_hWnd;
	ISpVoice * pSpVoice;
	CComPtr<ISpRecognizer> g_cpEngine;
	CComPtr<ISpRecoContext> cpRecoContext;
	CComPtr<ISpRecoGrammar> cpRecoGrammar;
	CComPtr<ISpRecoGrammar> cpDicGrammar;
	CComPtr<ISpRecoResult> cpRecoResult;
	CSpEvent                    spEvent;
	bool                        isDic;
	QWidget*                    m_Window;
	int                         system_state;
	std::map<ULONG,QString>        addressbook;

signals:
	void                        RouteStart();
	void                        RouteStop();
	void                        GetPath();
	void                        SetDestination(QString des, QString bldgName);
	void                        SetSource(QString source, QString bldgName);
	void                        UNRECOGNIZED(QString content);

public slots:
		void                        speak(QString) ;
		void                        startListening();//tell the machine to start listening
		void                        resumeListening();
		void                        pauseListening();//pause the listening
		void                        endListening();//end the listening, release resources.
		void                        phraseCommand(const QString& command, const QString& value = QString(""));//Phrase then send your command
		void                        ExecuteCommand( const ULONG ulRuleID, const ULONG ulVal, const QString& command = QString("") );
		void                        switchToDic();
		void                        switchToReco();
		void                        loadAddressbook();
};

void listenProcess(LPARAM lpParam);//the listening thread

#endif