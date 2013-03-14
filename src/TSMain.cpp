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

#include <QtGui/QApplication>
#include <QSettings>
#include <QFileInfo>

#include "TSBrowserApplication.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(speechnav);

	// Set QTWEBKIT_PLUGIN_PATH env to exe's path/plugin
	QFileInfo fi (argv[0]);
    QString appPath = fi.absolutePath();
	QString pluginPath = QString("%1/plugins").arg(appPath);
	qputenv("QTWEBKIT_PLUGIN_PATH", pluginPath.toAscii());

	// Add library path
	QApplication::addLibraryPath(QString("%1/plugins").arg(appPath));

   	// Read configurations
	QSettings settings("app_config.ini",QSettings::IniFormat);
	QString home_url(settings.value("system/HomeUrl", QVariant(QString())).toString());
	QString appTitle(settings.value("gui/AppTitle", QVariant(QString("TSWeb"))).toString());
	bool	bTitleBar(settings.value("gui/TitleBar", QVariant(false)).toBool());
	bool	bLoadMenu(settings.value("gui/LoadMenu", QVariant(true)).toBool());
	bool	bMinBtn(settings.value("gui/MinButton", QVariant(false)).toBool());
	bool	bMaxBtn(settings.value("gui/MaxButton", QVariant(false)).toBool());
	bool	bCloseBtn(settings.value("gui/CloseButton", QVariant(true)).toBool());
	QString logLevel(settings.value("log/logLevel", QVariant(QString("Info"))).toString());


	TSBrowserApplication app(argc, argv);

	if (!app.isTheOnlyBrowser())
        return 0;

	
	app.setLogLevel(logLevel); // Call this first
	app.enableTitleBar(bTitleBar);
	app.enableMinBtn(bMinBtn);
	app.enableMaxBtn(bMaxBtn);
	app.enableCloseBtn(bCloseBtn);
	app.enableLoadMenu(bLoadMenu);
	
	app.setStyle( new Qtitan::RibbonStyle()  );
	
	MyResourceMgr::initResourceMgr();

	app.setAppTile(appTitle);
	app.setHomeUrl(home_url);
	
	app.startApp();

	
	// Create main window	
    //TSMainWindow tsMainWin(url, appTitle);
	//tsMainWin.setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);   //Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint
	//tsMainWin.setWindowTitle(appTitle);
    
	//tsMainWin.showFullScreen();
	//tsMainWin.showMaximized(); 
	
	//tsMainWin.show();




    return app.exec();
}
