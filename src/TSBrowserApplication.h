// Copyright (C) T-Solution
//

//  
// File   : TSBrowserApplication.h
// Author : Zhan
//
#ifndef BROWSERAPPLICATION_H
#define BROWSERAPPLICATION_H

#include "MyResourceMgr.h"

#include <QtitanRibbon.h>

// QsLog: Qt Simple Logger
#include "QsLog.h"
#include "QsLogDest.h"

#include <QtGui/QApplication>

#include <QtCore/QUrl>
#include <QtCore/QPointer>

#include <QtGui/QIcon>

QT_BEGIN_NAMESPACE
class QLocalServer;
QT_END_NAMESPACE

class TSCookieJar;
class TSDownloadManager;
class TSNetworkAaccessManager;
class TSMainWindow;



class TSBrowserApplication : public QApplication
{
    Q_OBJECT

public:
    TSBrowserApplication(int &argc, char **argv);
    ~TSBrowserApplication();
    static TSBrowserApplication *instance();
    
	bool				isTheOnlyBrowser() const;
    QIcon				icon(const QUrl &url) const;

	void				startApp();

	inline void			setHomeUrl(const QString& url){this->homeUrl = url; QLOG_INFO() << QString("Program starting: Config Home URL to %1.").arg(url);}
	inline QString&		getHomeUrl() { return this->homeUrl;}

	
	inline void			setAppTile(const QString& title) {this->appTitle = title; QLOG_INFO() << QString("Program starting: Config Application Title to %1.").arg(title);};
	QString&			getAppTitle() { return this->appTitle;}

	
	void				setLogLevel(const QString& level);
	QsLogging::Level	getLogLevel(){return this->logLevel;};

	inline void			enableTitleBar(bool bLoad) { this->bTitleBar = bLoad; }
	inline bool			enableTitleBar() { return this->bTitleBar; }

	inline void			enableMinBtn(bool bLoad) { this->bMinBtn = bLoad; }
	inline bool			enableMinBtn() { return this->bMinBtn; }

	inline void			enableMaxBtn(bool bLoad) { this->bMaxBtn = bLoad; }
	inline bool			enableMaxBtn() { return this->bMaxBtn; }

	inline void			enableCloseBtn(bool bLoad) { this->bCloseBtn = bLoad; }
	inline bool			enableCloseBtn() { return this->bCloseBtn; }

	inline void			enableLoadMenu(bool bLoad) { this->bLoadMenu = bLoad; }
	inline bool			enableLoadMenu() { return this->bLoadMenu; }

	
	inline void			setOpenLinkInTab(bool bTab) { this->bOpenLinkInTab = bTab; }
	inline bool			getOpenLinkInTab() { return this->bOpenLinkInTab; }

	void				showDownloadManager(QObject *wid,bool _show = true);


	void				addNamedJSObject(QObject *wid, const QString& _name);
    
    static TSCookieJar *cookieJar();
    static TSNetworkAaccessManager *networkAccessManager();
	static TSDownloadManager *downloadManager();
	

	TSMainWindow		*newMainWindow(const QString& _url = QString(""), const QList<QVariant>& _menus = QList<QVariant>());
	QWidget				*mainWindow();
	QList<QPointer<TSMainWindow>> mainWindows();
    
#if defined(Q_WS_MAC)
    bool event(QEvent *event);
#endif

public slots:
	

#if defined(Q_WS_MAC)
    void lastWindowClosed();
    void quitBrowser();
#endif

private slots:
    void		postLaunch();
    void		newLocalSocketConnection();

private:
    void		clean();
    void		installTranslator(const QString &name);


private:
	static TSDownloadManager		*s_downloadManager;
	static TSNetworkAaccessManager	*s_networkAccessManager;
	
	QList<QPointer<TSMainWindow>>	m_mainWindows;

    QLocalServer					*m_localServer;
    mutable							QIcon m_defaultIcon;

	QString							homeUrl;
	QString							appTitle;
	QsLogging::Level				logLevel;
	QsLogging::DestinationPtr		logFileDestination;

	bool							bTitleBar;
	bool							bMinBtn;
	bool							bMaxBtn;
	bool							bCloseBtn;
	bool							bLoadMenu;
	bool							bOpenLinkInTab;
};

#endif // BROWSERAPPLICATION_H

