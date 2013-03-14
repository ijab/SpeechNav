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
#include "TSMainWindow.h"
#include "TSBrowserApplication.h"

#include <QMap>
#include <QVariant>
#include <QtGui>
#include <QStatusBar>
#include <QLabel>
#include <QActionGroup>

#include "MenuItemMgr.h"
#include "TSDownloadManager.h"
#include "TSDockWidgetTitleBar.h"


//--------------------------------------------------------------------------------------------------------
TSMainWindow::TSMainWindow( const QString& url, QWidget* parent ) 
 : Qtitan::RibbonMainWindow(parent),
   strURL( url )
 , myActionGroup(0)
{

	m_ribbonStyle = qobject_cast<Qtitan::RibbonStyle*>(qApp->style());

    myResMgr = new MyResourceMgr( TSBrowserApplication::instance()->getAppTitle() , QString( "%1Config" ) );

	myActionGroup = new QActionGroup( this ) ;


	// Create Menu, tool bar and file transer/uploading dockwidget
	createMenuFile();
	createRibbon();
	createStatusBar();	

	QAction* actionAbout = ribbonBar()->addAction( myResMgr->loadPixmap("/Resources/about.png"), "About", Qt::ToolButtonIconOnly);
    actionAbout->setToolTip(tr("iJab Copyright."));
    connect(actionAbout, SIGNAL(triggered()), this, SLOT(onAbout()));

	actionRibbonMinimize = ribbonBar()->addAction(  myResMgr->loadPixmap("/Resources/ribbonMinimize.png"), "Minimize the Ribbon", Qt::ToolButtonIconOnly);
    actionRibbonMinimize->setStatusTip(tr("Show only the tab names on the Ribbon"));
    actionRibbonMinimize->setShortcut(tr("Ctrl+F1"));
    //connect(actionRibbonMinimize, SIGNAL(triggered()), this, SLOT(maximizeToggle()));
    connect(ribbonBar(), SIGNAL(minimizationChanged(bool)), this, SLOT(minimizationChanged(bool)));
	
	maximizeToggle();
	minimizationChanged( true ) ;
	
	// Create TSWebViewer
    webViewer = new TSWebViewer(this, url, false);
    setCentralWidget(webViewer);

	connectToWebView();

	this->loadURL( url );

    setUnifiedTitleAndToolBarOnMac(true);

	downloadManagerDock = 0;
}

TSMainWindow::~TSMainWindow(){

	if(myResMgr)
	{
		delete myResMgr;
		myResMgr = 0 ;
	}
	
}

void TSMainWindow::loadURL( const QString &url ) 
{
	if( webViewer )
	{
		if( urlLabel )
			urlLabel->setText( url );

		webViewer->loadURL( url );
	}
}

void TSMainWindow::loadMenus() 
{
	if( TSBrowserApplication::instance()->enableLoadMenu() )
		MenuItemMgr::instance()->updateRibbonMenu( ribbonBar() ) ;
}

MyResourceMgr* TSMainWindow::resourceMgr() 
{
	return myResMgr ;
}


void TSMainWindow::onMenuAction ( QAction *action )
{
	QVariant _d = action->data();
	QMap<QString, QVariant> _dmap = _d.toMap();

	if( !_dmap.isEmpty() )
	{
		
	}	
}

//=============================================================================================================

void TSMainWindow::createMenuFile()
{
    QIcon iconLogo;
    iconLogo.addPixmap( myResMgr->loadPixmap("/Resources/qtitan.png"));
    iconLogo.addPixmap( myResMgr->loadPixmap("/Resources/qtitanlogo32x32.png"));
    if (QAction* actionFile = ribbonBar()->addSystemButton(iconLogo, tr("&File"))) 
    {
        actionFile->setToolTip(tr("File Menu"));

        if (popupBar = qobject_cast<Qtitan::RibbonSystemPopupBar*>(actionFile->menu()))
        {
            /*QAction* newFile = popupBar->addAction(myResMgr->loadPixmap("/res/new.png"), tr("&New"));
            newFile->setShortcut(tr("Ctrl+N"));
            newFile->setStatusTip(tr("Create a new document"));
            newFile->setToolTip(tr("New"));
            newFile->setEnabled(false);*/

            QIcon iconClose;
            iconClose.addPixmap(myResMgr->loadPixmap("/Resources/exit.png"));
            iconClose.addPixmap(myResMgr->loadPixmap("/Resources/smallexit.png"));
            QAction* actClose = popupBar->addAction(iconClose, tr("&Exit"));
            actClose->setShortcut(tr("Ctrl+E"));
            actClose->setStatusTip(tr("Exit"));
            connect(actClose, SIGNAL(triggered()), this, SLOT(close()));
            popupBar->addPopupBarAction(actClose, Qt::ToolButtonTextBesideIcon);
        }
    }
}

void TSMainWindow::createRibbon()
{
	if( TSBrowserApplication::instance()->enableLoadMenu() )
	{
		// Add webkit actions
		if (Qtitan::RibbonPage* pageButtons = ribbonBar()->addPage(tr("&Navigate")))
		{
			Qtitan::RibbonGroup* groupWebkit = pageButtons->addGroup(tr(""));
			if (groupWebkit) 
			{
	            
				QAction* action = groupWebkit->addAction(myResMgr->loadPixmap("/Resources/home.png"), 
					tr("&Home"), Qt::ToolButtonTextUnderIcon);
				connect(action, SIGNAL(triggered()), this, SLOT(onHome()));

				action = groupWebkit->addAction(myResMgr->loadPixmap("/Resources/back.png"), 
					tr("&Back"), Qt::ToolButtonTextUnderIcon);
				connect(action, SIGNAL(triggered()), this, SLOT(onBack()));

				action = groupWebkit->addAction(myResMgr->loadPixmap("/Resources/forward.png"), 
					tr("&Forward"), Qt::ToolButtonTextUnderIcon);
				connect(action, SIGNAL(triggered()), this, SLOT(onBack()));

				action = groupWebkit->addAction(myResMgr->loadPixmap("/Resources/settings.png"), 
					tr("&Settings"), Qt::ToolButtonTextUnderIcon);
				connect(action, SIGNAL(triggered()), this, SLOT(onSettings()));


				//action = groupWebkit->addAction(myResMgr->loadPixmap("/Resources/about.png"), 
				//    tr("&About"), Qt::ToolButtonTextUnderIcon);
				//connect(action, SIGNAL(triggered()), this, SLOT(onAbout()));
	     
				
			}
		}

		// Add view menus
		if (Qtitan::RibbonPage* pageButtons = ribbonBar()->addPage(tr("&View")))
		{
			groupView = pageButtons->addGroup(tr(""));       
		}
	}
	ribbonBar()->setFrameThemeEnabled();
}



void TSMainWindow::createStatusBar()
{
	QStatusBar *statusBar = this->statusBar();

	progressBar = new QProgressBar(statusBar);
	urlLabel = new QLabel( tr( " URL " ) );
	statusLabel = new QLabel( tr(" Status ") );

	statusLabel->setMinimumSize( statusLabel->sizeHint() );
	statusLabel->setAlignment( Qt::AlignCenter );
	statusLabel->setText( tr("Ready") );
	statusLabel->setToolTip( tr("Status") );

	statusBar->addWidget( statusLabel );

	urlLabel->setMinimumSize( urlLabel->sizeHint() );
	urlLabel->setAlignment( Qt::AlignCenter );
	urlLabel->setText( tr("URL") );
	urlLabel->setToolTip( tr("URL To Load") );

	statusBar->addWidget( urlLabel );

	
	progressBar->setTextVisible( false );
	progressBar->setRange( 0, 100 );

	statusBar->addWidget( progressBar, 1 );
}

void TSMainWindow::showDownloadManager(TSDownloadManager *dmWidget, bool bShow)
{
	if( !dmWidget ) return;

	if( !this->downloadManagerDock )
	{
		this->downloadManagerDock = new QDockWidget(tr("Download Manager"), this);

		TSDockWidgetTitleBar *titleBar = 0;
        // Check if the dock widget is supposed to be collapsable
        if (!this->downloadManagerDock->titleBarWidget()) {
            titleBar = new TSDockWidgetTitleBar(this->downloadManagerDock);
            this->downloadManagerDock->setTitleBarWidget(titleBar);
            titleBar->setCollapsable(false);
        }

        this->downloadManagerDock->setObjectName("Upload Manager");

        if (this->downloadManagerDock->widget() && this->downloadManagerDock->widget()->layout())
            this->downloadManagerDock->widget()->layout()->setContentsMargins(1, 1, 1, 1);

		this->downloadManagerDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
		this->downloadManagerDock->setFeatures( QDockWidget::DockWidgetClosable |  QDockWidget::DockWidgetMovable 
											|  QDockWidget::DockWidgetFloatable );
		addDockWidget(Qt::RightDockWidgetArea, this->downloadManagerDock);

		this->downloadManagerDock->setFloating(false);
	
		if (titleBar)
            titleBar->setCollapsed(false);
	}

	
	this->downloadManagerDock->setWidget(dmWidget);
	this->downloadManagerDock->setVisible(bShow);
}



void TSMainWindow::connectToWebView()
{
	if( webViewer )
	{
		connect(webViewer, SIGNAL(loadStarted()), SLOT(onLoadStarted()));
		connect(webViewer, SIGNAL(titleChanged(QString)), SLOT(onTitleChanged(QString)));
		connect(webViewer, SIGNAL(loadFinished(bool)), SLOT(onLoadFinished(bool)));
		connect(webViewer, SIGNAL(statusBarMessage(QString)), SLOT(onStatusBarMessage(QString)));
		connect(webViewer, SIGNAL(loadProgress(int)), SLOT(onLoadProgress(int)));
	}
}

void TSMainWindow::maximizeToggle()
{
    ribbonBar()->setMinimized(!ribbonBar()->isMinimized());
}

void TSMainWindow::minimizationChanged(bool minimized)
{
    actionRibbonMinimize->setChecked(minimized);
    actionRibbonMinimize->setIcon(minimized ? myResMgr->loadPixmap("/Resources/ribbonMaximize.png") :  myResMgr->loadPixmap("/Resources/ribbonMinimize.png"));
}

void TSMainWindow::onLoadStarted()
{
	if( webViewer ) 
	{
		if( statusLabel )
			statusLabel->setText( tr( "Loading ..." ) );

		if( urlLabel )
			urlLabel->setText( strURL );
	}
}


void TSMainWindow::onLoadSucceeded()
{
	if( statusLabel )
		statusLabel->setText( tr( "Completed" ) );
}


void TSMainWindow::onTitleChanged(QString title)
{
	this->setWindowTitle( QString("%1 - %2").arg(TSBrowserApplication::instance()->getAppTitle()).arg(title));
}

void TSMainWindow::onLoadProgress(int progress)
{
	if( progressBar )
	{
		progressBar->show();
		progressBar->setValue( progress );
	}
}

void TSMainWindow::onStatusBarMessage( QString status )
{
	if( statusLabel )
		statusLabel->setText( status );

}

void TSMainWindow::onLoadFinished(bool bSucc)
{
	if( progressBar ) progressBar->hide();

	if( !bSucc )
	{
		if( statusLabel )
			statusLabel->setText( tr( "Error Occured" ) );
	}
	else
	{
		if( statusLabel )
			statusLabel->setText( tr( "Completed" ) );
	}
}


void TSMainWindow::onAbout()
{
    QMessageBox::about(this, tr("About SpeechNav"),
        tr("The <b>SpeechNav</b> works great "
           "with Google Map & Windows Speech SDK."));

}

void TSMainWindow::onHome()
{
	this->loadURL( strURL ) ;
}

void TSMainWindow::onForward()
{
	this->webViewer->forward();

}

void TSMainWindow::onBack()
{
	this->webViewer->back();
}

void TSMainWindow::onSettings()
{
	QMessageBox::information( this , "Debug" , "OnSettings" ) ;
}