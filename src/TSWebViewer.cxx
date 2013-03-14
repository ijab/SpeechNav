// Copyright (C) T-Solution
//

// File   : TSWebViewer.cxx
// Author : Zhan
//
#include "TSWebViewer.h"
#include "MSSpeech.h"
#include "TSDownloadManager.h"
#include "TSNetworkAccessManager.h"
#include "TSBrowserApplication.h"

#include <QVBoxLayout>
#include <QWebFrame>
#include <QNetworkReply>
#include <QStyle>

#include <QtCore/QFile>

#include <QtCore/QDebug>
#include <QtCore/QBuffer>

TSWebPage::TSWebPage(QObject *parent)
    : QWebPage(parent)
{
    setNetworkAccessManager(dynamic_cast<QNetworkAccessManager*>(TSBrowserApplication::networkAccessManager()));
    connect(this, SIGNAL(unsupportedContent(QNetworkReply*)),
            this, SLOT(handleUnsupportedContent(QNetworkReply*)));
	connect(this, SIGNAL(featurePermissionRequested(QWebFrame*, QWebPage::Feature)), 
			this, SLOT(permissionRequested(QWebFrame*, QWebPage::Feature)));
}

void TSWebPage::permissionRequested(QWebFrame* frame, QWebPage::Feature feature)
{
    setFeaturePermission(frame, feature, PermissionGrantedByUser);
}

void TSWebPage::handleUnsupportedContent(QNetworkReply *reply)
{
    QString errorString = reply->errorString();

    if (m_loadingUrl != reply->url()) {
        // sub resource of this page
        qWarning() << "Resource" << reply->url().toEncoded() << "has unknown Content-Type, will be ignored.";
        reply->deleteLater();
        return;
    }

    if (reply->error() == QNetworkReply::NoError && !reply->header(QNetworkRequest::ContentTypeHeader).isValid()) {
        errorString = "Unknown Content-Type";
    }

    QFile file(QLatin1String(":/Resources/notfound.html"));
    bool isOpened = file.open(QIODevice::ReadOnly);
    Q_ASSERT(isOpened);
    Q_UNUSED(isOpened)

    QString title = tr("Error loading page: %1").arg(reply->url().toString());
    QString html = QString(QLatin1String(file.readAll()))
                        .arg(title)
                        .arg(errorString)
                        .arg(reply->url().toString());

    QBuffer imageBuffer;
    imageBuffer.open(QBuffer::ReadWrite);
    QIcon icon = view()->style()->standardIcon(QStyle::SP_MessageBoxWarning, 0, view());
    QPixmap pixmap = icon.pixmap(QSize(32,32));
    if (pixmap.save(&imageBuffer, "PNG")) {
        html.replace(QLatin1String("IMAGE_BINARY_DATA_HERE"),
                     QString(QLatin1String(imageBuffer.buffer().toBase64())));
    }

    QList<QWebFrame*> frames;
    frames.append(mainFrame());
    while (!frames.isEmpty()) {
        QWebFrame *frame = frames.takeFirst();
        if (frame->url() == reply->url()) {
            frame->setHtml(html, reply->url());
            return;
        }
        QList<QWebFrame *> children = frame->childFrames();
        foreach(QWebFrame *frame, children)
            frames.append(frame);
    }
    if (m_loadingUrl == reply->url()) {
        mainFrame()->setHtml(html, reply->url());
    }
}

bool TSWebPage::acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest &request, NavigationType type)
{
    if (frame == mainFrame()) {
        m_loadingUrl = request.url();
        emit loadingUrl(m_loadingUrl);
    }
    return QWebPage::acceptNavigationRequest(frame, request, type);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TSWebViewer inherites from QWebView
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*!
  \brief Constructor.
 
  Construct the web browser.
*/

TSWebViewer::TSWebViewer( QWidget* parent, const QString& url, bool bLoadUrl ) 
: QWebView(parent)
, m_page(new TSWebPage(this))
{
	this->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));

	setPage(m_page);
	page()->setForwardUnsupportedContent(true);

	myProxyJSObject = 0;	

	connect(page(), SIGNAL(downloadRequested(QNetworkRequest)),
            this, SLOT(downloadRequested(QNetworkRequest)));

	// Connect to script object
	connect( this->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
            this, SLOT(addJavaScriptWindowObject()));

	// Settings for QWebView
	this->settings()->setAttribute(QWebSettings::PluginsEnabled, true);

	this->settings()->setAttribute(QWebSettings::JavascriptEnabled, true);

#ifdef _DEBUG
	this->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
#endif
	this->settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);

	//this->setContextMenuPolicy(Qt::NoContextMenu);

	if( !url.isEmpty() && bLoadUrl )
		this->loadURL(url);
}

/*!
  \brief Destructor.
*/
TSWebViewer::~TSWebViewer()
{
	if( myProxyJSObject )
	{
		delete myProxyJSObject;
		myProxyJSObject = 0;
	}
}


/*!
  \brief Get popup menu client type.
  \return popup client type
*/
QString TSWebViewer::popupClientType() const
{
  return "TSWebViewer";
}


/*!
  \brief Load given url address and optional scroll to the specified anchor
  \param url an url address to load
  \param anchor an anchor to scroll page to
*/
void TSWebViewer::loadURL( const QString& url, const QString& anchor )
{
	QString anUrl = url;
	if( !anchor.isEmpty() ) anUrl += "#" + anchor;
		anUrl.replace('\\', '/');

	this->load( QUrl( anUrl ) );	
}

void TSWebViewer::addNamedJSObject(const QString& name)
{
	if( name.isEmpty() || name.isNull() ) return;

	TSWebProxyObject *_jsObject = new TSWebProxyObject(this);
	this->page()->mainFrame()->addToJavaScriptWindowObject(name, dynamic_cast<QObject*>(_jsObject));
}

void TSWebViewer::addJavaScriptWindowObject()
{
	if( myProxyJSObject )
	{
		delete myProxyJSObject;
		myProxyJSObject = 0;
	}
	myProxyJSObject = new TSWebProxyObject(this);
	myProxyJSObject->startListening();
	this->page()->mainFrame()->addToJavaScriptWindowObject( "tsWebProxyObject", dynamic_cast<QObject*>(myProxyJSObject) );
}

void TSWebViewer::downloadRequested(const QNetworkRequest &request)
{
    TSBrowserApplication::downloadManager()->download(request);
}

QWebView* TSWebViewer::createWindow(QWebPage::WebWindowType type)
{
	QWebView *webview = new TSWebViewer();
    // You insert it somehow in another window if necessary
    // or just show it as a standalone window
    webview->show();
    return webview;
}