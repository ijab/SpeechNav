/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef TSMAINWINDOW_H
#define TSMAINWINDOW_H

#include "TSWebApp.h"

#include <QProgressBar>

#include "TSWebViewer.h"
#include "MyResourceMgr.h"

#include <QtitanRibbon.h>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
QT_END_NAMESPACE

class MyResourceMgr;
class TSDownloadManager;

namespace Qtitan 
{ 
    class WidgetGalleryItems; 
    class OfficeWidgetGallery;
}

class TSWEBAPP_EXPORTS TSMainWindow : public Qtitan::RibbonMainWindow
{
    Q_OBJECT

public:
    TSMainWindow( const QString& url = QString(), QWidget* parent = Q_NULL );
	~TSMainWindow();

	void				loadURL( const QString& url );

	void				loadMenus() ;
	
	void				showDownloadManager(TSDownloadManager *dmWidget, bool bShow = true);
	QDockWidget*		getDownloadManager() { return this->downloadManagerDock;};


	MyResourceMgr*		resourceMgr();

	QActionGroup*		getActionGroup() { return myActionGroup ;};

	TSWebViewer*		currentWebViewr() { return this->webViewer;};

	
public slots:
	void				onMenuAction ( QAction * action ) ;

private slots:
	void				onHome();
	void				onForward() ;
	void				onBack() ;
	void				onSettings() ;
    void				onAbout();

	void				onLoadStarted();
    void				onLoadSucceeded();
    void				onTitleChanged(QString);
	void				onLoadProgress(int);
	void				onStatusBarMessage(QString);
	void				onLoadFinished(bool);

	void				maximizeToggle();
    void				minimizationChanged(bool minimized);


private:
	void				createMenuFile();
    void				createRibbon();
	void				createStatusBar();
	void				connectToWebView();	

private:
    TSWebViewer			*webViewer;
	
	QAction				*actionRibbonMinimize;

	QLabel				*statusLabel;
	QLabel				*urlLabel;
	QProgressBar		*progressBar;
	QString				strURL;

	//Qtitan::RibbonPage* m_pageContext;
    Qtitan::RibbonStyle* m_ribbonStyle;

	MyResourceMgr		*myResMgr ;

	QActionGroup		*myActionGroup ;

	Qtitan::RibbonGroup *groupView;
	QDockWidget			*downloadManagerDock;
	Qtitan::RibbonSystemPopupBar* popupBar;
    
};

#endif // MAINWINDOW_H
