// Copyright (C) T-Solution
//

//  
// File   : TSWebViewer.h
// Author : Zhan
//
#ifndef TSWEBVIEWER_H
#define TSWEBVIEWER_H

#include "TSWebApp.h"

#include <QWidget>
#include <QWebView>
#include <QWebPage>


#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QNetworkRequest;
class TSWebProxyObject;

// To set our own networkacessmanager
class TSWebPage : public QWebPage {
    Q_OBJECT

signals:
	void loadingUrl(const QUrl &url);

public:
    TSWebPage(QObject *parent = 0);

protected:
	bool acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest &request, NavigationType type);

private slots:
    void handleUnsupportedContent(QNetworkReply *reply);
	void permissionRequested(QWebFrame* frame, QWebPage::Feature feature);        

private:
	QUrl m_loadingUrl;  
	friend class TSWebView;
};


// To reimplement createWindow function of QWebView
class TSWEBAPP_EXPORTS TSWebViewer : public QWebView
{
  Q_OBJECT


public:
	explicit TSWebViewer(QWidget* parent = 0, const QString& url = QString(), bool bLoadUrl = false);
	virtual ~TSWebViewer();

	TSWebPage					*webPage() const { return m_page; }
    
	virtual void				loadURL(const QString& url, const QString& anchor = QString(""));
	virtual QString				popupClientType() const;
	
	void						addNamedJSObject(const QString& name);

protected:
	virtual QWebView*			createWindow(QWebPage::WebWindowType type);


private slots:
	void						addJavaScriptWindowObject();
	void						downloadRequested(const QNetworkRequest &request);

private:
	TSWebProxyObject			*myProxyJSObject;

	TSWebPage					*m_page;

};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif // TSWEBVIEWER_H
