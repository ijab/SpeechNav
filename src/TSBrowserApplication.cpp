// Copyright (C) T-Solution
//

//  
// File   : TSBrowserApplication.cpp
// Author : Zhan
//

#include "TSBrowserApplication.h"
#include "TSCookieJar.h"
#include "TSNetworkAccessManager.h"
#include "TSDownloadManager.h"
#include "TSMainWindow.h"
#include "MenuItemMgr.h"


#include <QtCore/QBuffer>
#include <QtCore/QDir>
#include <QtCore/QLibraryInfo>
#include <QtCore/QSettings>
#include <QtCore/QTextStream>
#include <QtCore/QTranslator>
#include <QtCore/QFileInfo>

#include <QtGui/QDesktopServices>
#include <QtGui/QFileOpenEvent>
#include <QtGui/QMessageBox>

#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>
#include <QtNetwork/QNetworkProxy>
#include <QtNetwork/QSslSocket>

#include <QtWebKit/QWebSettings>

#include <QtCore/QDebug>

TSDownloadManager	*TSBrowserApplication::s_downloadManager = 0;
TSNetworkAaccessManager *TSBrowserApplication::s_networkAccessManager = 0;

TSBrowserApplication::TSBrowserApplication(int &argc, char **argv)
    : QApplication(argc, argv)
    , m_localServer(0)
	, bOpenLinkInTab(false)
{
    QCoreApplication::setOrganizationName(QLatin1String("IJAB"));
    QCoreApplication::setApplicationName(QLatin1String("SpeechNav"));
    QCoreApplication::setApplicationVersion(QLatin1String("1.0.0"));
#ifdef Q_WS_QWS
    // Use a different server name for QWS so we can run an X11
    // browser and a QWS browser in parallel on the same machine for
    // debugging
    QString serverName = QCoreApplication::applicationName() + QLatin1String("_qws");
#else
    QString serverName = QCoreApplication::applicationName();
#endif
    QLocalSocket socket;
    socket.connectToServer(serverName);
    if (socket.waitForConnected(500)) {
        QTextStream stream(&socket);
        QStringList args = QCoreApplication::arguments();
        if (args.count() > 1)
            stream << args.last();
        else
            stream << QString();
        stream.flush();
        socket.waitForBytesWritten();
        return;
    }

#if defined(Q_WS_MAC)
    QApplication::setQuitOnLastWindowClosed(false);
#else
    QApplication::setQuitOnLastWindowClosed(true);
#endif

    m_localServer = new QLocalServer(this);
    connect(m_localServer, SIGNAL(newConnection()),
            this, SLOT(newLocalSocketConnection()));

    if (!m_localServer->listen(serverName)) {
        if (m_localServer->serverError() == QAbstractSocket::AddressInUseError
            && QFile::exists(m_localServer->serverName())) {
            QFile::remove(m_localServer->serverName());
            m_localServer->listen(serverName);
        }
    }

#ifndef QT_NO_OPENSSL
    if (!QSslSocket::supportsSsl()) {
    QMessageBox::information(0, "SpeechNav",
                 "This system does not support OpenSSL. SSL websites will not be available.");
    }
#endif

    QDesktopServices::setUrlHandler(QLatin1String("http"), this, "openUrl");
    QString localSysName = QLocale::system().name();

    installTranslator(QLatin1String("qt_") + localSysName);

#if defined(Q_WS_MAC)
    connect(this, SIGNAL(lastWindowClosed()),
            this, SLOT(lastWindowClosed()));
#endif

    QTimer::singleShot(0, this, SLOT(postLaunch()));

	// Set logger
	QsLogging::Logger& logger = QsLogging::Logger::instance();
	
	const QString sLogPath(QDir(applicationDirPath()).filePath("tsweb.log"));
	
	logFileDestination = QsLogging::DestinationFactory::MakeFileDestination(sLogPath);
	logger.addDestination(logFileDestination.get());
}

TSBrowserApplication::~TSBrowserApplication()
{
	delete s_downloadManager;
    delete s_networkAccessManager;
}

#if defined(Q_WS_MAC)
void TSBrowserApplication::lastWindowClosed()
{
    clean();
}
#endif

TSBrowserApplication *TSBrowserApplication::instance()
{
    return (static_cast<TSBrowserApplication *>(QCoreApplication::instance()));
}

#if defined(Q_WS_MAC)
#include <QtGui/QMessageBox>
void TSBrowserApplication::quitBrowser()
{
    clean();
    exit(0);
}
#endif

/*!
    Any actions that can be delayed until the window is visible
 */
void TSBrowserApplication::postLaunch()
{
    QString directory = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    if (directory.isEmpty())
        directory = QDir::homePath() + QLatin1String("/.") + QCoreApplication::applicationName();
    QWebSettings::setIconDatabasePath(directory);
    QWebSettings::setOfflineStoragePath(directory);

    setWindowIcon(QIcon(QLatin1String(":/Resources/qtitanlogo32x32.png")));
}

void TSBrowserApplication::clean()
{
}



bool TSBrowserApplication::isTheOnlyBrowser() const
{
    return (m_localServer != 0);
}

void TSBrowserApplication::installTranslator(const QString &name)
{
    QTranslator *translator = new QTranslator(this);
    translator->load(name, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    QApplication::installTranslator(translator);
}

#if defined(Q_WS_MAC)
bool TSBrowserApplication::event(QEvent* event)
{
    switch (event->type()) {
    case QEvent::ApplicationActivate: {
        clean();
        return true;
    }
    case QEvent::FileOpen:
        static_cast<QFileOpenEvent *>(event)->file();
        return true;        
    default:
        break;
    }
    return QApplication::event(event);
}
#endif


void TSBrowserApplication::newLocalSocketConnection()
{
    QLocalSocket *socket = m_localServer->nextPendingConnection();
    if (!socket)
        return;
    socket->waitForReadyRead(1000);
    QTextStream stream(socket);
    QString url;
    stream >> url;
    
    delete socket;
}

TSCookieJar *TSBrowserApplication::cookieJar()
{
    return (TSCookieJar*)networkAccessManager()->cookieJar();
}


TSDownloadManager *TSBrowserApplication::downloadManager()
{
    if (!s_downloadManager) {
		s_downloadManager = new TSDownloadManager();
    }
    return s_downloadManager;
}

TSNetworkAaccessManager *TSBrowserApplication::networkAccessManager()
{
    if (!s_networkAccessManager) {
        s_networkAccessManager = new TSNetworkAaccessManager();
        s_networkAccessManager->setCookieJar(new TSCookieJar);
    }
    return s_networkAccessManager;
}



TSMainWindow* TSBrowserApplication::newMainWindow(const QString& _url, const QList<QVariant>& _menus)
{
	QString _tmpUrl = _url;

	if ( _url.isEmpty() || _url.isNull() )
	{
		_tmpUrl = this->getHomeUrl();
		
	}
			

	if(	(this->getOpenLinkInTab() && m_mainWindows.count() < 1) 
		|| !this->getOpenLinkInTab())
	{
		// If no main window, create it first
		TSMainWindow *mw = new TSMainWindow(_tmpUrl);
		mw->setWindowTitle(appTitle);

		MenuItemMgr::instance()->setMainWindow(mw);
		bool ret = MenuItemMgr::instance()->loadMenuItems(_menus) ;
		
		mw->loadMenus() ; // dynamically load menus
		
		// add to windows list
		m_mainWindows.push_front(mw);	
		
	}
	
	// Only show file upload and download manager in one window if not using tab-mode
	// Show upload and download manager in main window with tab-mode
	if( (!this->getOpenLinkInTab() && m_mainWindows.count() == 1)
		|| this->getOpenLinkInTab())
	{
		if( !m_mainWindows[0]->getDownloadManager() )
			m_mainWindows[0]->showDownloadManager(TSBrowserApplication::instance()->downloadManager(), false);
	}

	m_mainWindows[0]->showMaximized();
	
	return m_mainWindows[0];
}

QWidget *TSBrowserApplication::mainWindow()
{
	// Returen current main window of the application
	if (m_mainWindows.count() < 1) {
		return this->newMainWindow();
    }
	else
		return m_mainWindows[0];
}

QList<QPointer<TSMainWindow>> TSBrowserApplication::mainWindows()
{
	return m_mainWindows;
}


void TSBrowserApplication::showDownloadManager(QObject *wid, bool _show/* = true*/)
{
	if( !wid ) return;
	
	QObject *p = wid;
	TSMainWindow *_mw = 0;

	while(p)
	{	
		_mw = dynamic_cast<TSMainWindow*>(p);
	
		if( _mw ) 
		{
			_mw->showDownloadManager(TSBrowserApplication::instance()->downloadManager(), _show);
			return;
		}

		p = p->parent();
	}		
}


void TSBrowserApplication::addNamedJSObject(QObject *wid, const QString& _name)
{
	if( !wid ) return;

	if( _name.isEmpty() || _name.isNull() ) return;

	QObject *p = wid;
	TSMainWindow *_mw = 0;
	TSWebViewer	*_wv = 0;
	
	while(p)
	{	
		_wv = dynamic_cast<TSWebViewer*>(p);
		if( _wv ) 
		{
			_wv->addNamedJSObject(_name);
			return;
		}

		_mw = dynamic_cast<TSMainWindow*>(p);	
		if( _mw ) 
		{
			_mw->currentWebViewr()->addNamedJSObject(_name);
			return;
		}

		p = p->parent();
	}	
}

QIcon TSBrowserApplication::icon(const QUrl &url) const
{
    QIcon icon = QWebSettings::iconForUrl(url);
    if (!icon.isNull())
        return icon.pixmap(16, 16);
    if (m_defaultIcon.isNull())
        m_defaultIcon = QIcon(QLatin1String(":/Resources/defaulticon.png"));
    return m_defaultIcon.pixmap(16, 16);
}

void TSBrowserApplication::setLogLevel(const QString& level) 
{
	//Trace|Debug|Info|Warn|Error|Fatal|None  default value is Info
	if( !level.compare("Trace", Qt::CaseInsensitive) )
		this->logLevel = QsLogging::TraceLevel;
	else if( !level.compare("Debug", Qt::CaseInsensitive) )
		this->logLevel = QsLogging::DebugLevel;
	else if( !level.compare("Info", Qt::CaseInsensitive) )
		this->logLevel = QsLogging::InfoLevel;
	else if( !level.compare("Warn", Qt::CaseInsensitive) )
		this->logLevel = QsLogging::WarnLevel;
	else if( !level.compare("Error", Qt::CaseInsensitive) )
		this->logLevel = QsLogging::ErrorLevel;
	else if( !level.compare("Fatal", Qt::CaseInsensitive) )
		this->logLevel = QsLogging::FatalLevel;
	else if( !level.compare("None", Qt::CaseInsensitive) )
		this->logLevel = QsLogging::NoneLevel;
	else		// default is Info
		this->logLevel = QsLogging::InfoLevel;

	// Configure Logger
	QsLogging::Logger& logger = QsLogging::Logger::instance();
	logger.setLoggingLevel(this->getLogLevel());	
	
	QLOG_INFO() << QString("Program starting: Config log level to %1 and log into file %2.")
					.arg(level)
					.arg(QString(QDir(applicationDirPath()).filePath("tsweb.log")));
}


void TSBrowserApplication::startApp()
{
	QWidget *_mw = this->mainWindow();
	
	_mw->show();

	QLOG_INFO() << QString("Program starting: Show main window.");
}
