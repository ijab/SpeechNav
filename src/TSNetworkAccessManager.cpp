// Copyright (C) T-Solution
//

//  
// File   : TSNetworkAccessManager.cpp
// Author : Zhan
//

#include "TSNetworkAccessManager.h"

#include "TSBrowserApplication.h"
#include "ui_passworddialog.h"
#include "ui_proxy.h"

#include <QtCore/QSettings>

#include <QtGui/QDesktopServices>
#include <QtGui/QDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QStyle>
#include <QtGui/QTextDocument>

#include <QtNetwork/QAuthenticator>
#include <QtNetwork/QNetworkDiskCache>
#include <QtNetwork/QNetworkProxy>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QSslError>

TSNetworkAaccessManager::TSNetworkAaccessManager(QObject *parent)
    : QNetworkAccessManager(parent),
    requestFinishedCount(0), requestFinishedFromCacheCount(0), requestFinishedPipelinedCount(0),
    requestFinishedSecureCount(0)
{
    connect(this, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
            SLOT(authenticationRequired(QNetworkReply*,QAuthenticator*)));
    connect(this, SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)),
            SLOT(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)));
    connect(this, SIGNAL(finished(QNetworkReply*)),
            SLOT(requestFinished(QNetworkReply*)));
#ifndef QT_NO_OPENSSL
    connect(this, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)),
            SLOT(sslErrors(QNetworkReply*,QList<QSslError>)));
#endif
    loadSettings();

    QNetworkDiskCache *diskCache = new QNetworkDiskCache(this);
    QString location = QDesktopServices::storageLocation(QDesktopServices::CacheLocation);
    diskCache->setCacheDirectory(location);
    setCache(diskCache);
}

QNetworkReply* TSNetworkAaccessManager::createRequest(Operation op, const QNetworkRequest & req, QIODevice * outgoingData)
{
    QNetworkRequest request = req; // copy so we can modify
    // this is a temporary hack until we properly use the pipelining flags from QtWebkit
    // pipeline everything! :)
    request.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
    return QNetworkAccessManager::createRequest(op, request, outgoingData);
}

void TSNetworkAaccessManager::requestFinished(QNetworkReply *reply)
{
    requestFinishedCount++;

    if (reply->attribute(QNetworkRequest::SourceIsFromCacheAttribute).toBool() == true)
        requestFinishedFromCacheCount++;

    if (reply->attribute(QNetworkRequest::HttpPipeliningWasUsedAttribute).toBool() == true)
        requestFinishedPipelinedCount++;

    if (reply->attribute(QNetworkRequest::ConnectionEncryptedAttribute).toBool() == true)
        requestFinishedSecureCount++;

    if (requestFinishedCount % 10)
        return;

    double pctCached = (double(requestFinishedFromCacheCount) * 100.0/ double(requestFinishedCount));
    double pctPipelined = (double(requestFinishedPipelinedCount) * 100.0/ double(requestFinishedCount));
    double pctSecure = (double(requestFinishedSecureCount) * 100.0/ double(requestFinishedCount));
#ifdef QT_DEBUG
    qDebug("STATS [%lli requests total] [%3.2f%% from cache] [%3.2f%% pipelined] [%3.2f%% SSL/TLS]", requestFinishedCount, pctCached, pctPipelined, pctSecure);
#endif
}

void TSNetworkAaccessManager::loadSettings()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("proxy"));
    QNetworkProxy proxy;
    if (settings.value(QLatin1String("enabled"), false).toBool()) {
        if (settings.value(QLatin1String("type"), 0).toInt() == 0)
            proxy = QNetworkProxy::Socks5Proxy;
        else
            proxy = QNetworkProxy::HttpProxy;
        proxy.setHostName(settings.value(QLatin1String("hostName")).toString());
        proxy.setPort(settings.value(QLatin1String("port"), 1080).toInt());
        proxy.setUser(settings.value(QLatin1String("userName")).toString());
        proxy.setPassword(settings.value(QLatin1String("password")).toString());
    }
    setProxy(proxy);
}

void TSNetworkAaccessManager::authenticationRequired(QNetworkReply *reply, QAuthenticator *auth)
{
    QWidget *mainWindow = TSBrowserApplication::instance()->mainWindow();

    QDialog dialog(mainWindow);
    dialog.setWindowFlags(Qt::Sheet);

    Ui::PasswordDialog passwordDialog;
    passwordDialog.setupUi(&dialog);

    passwordDialog.iconLabel->setText(QString());
    passwordDialog.iconLabel->setPixmap(mainWindow->style()->standardIcon(QStyle::SP_MessageBoxQuestion, 0, mainWindow).pixmap(32, 32));

    QString introMessage = tr("<qt>Enter username and password for \"%1\" at %2</qt>");
    introMessage = introMessage.arg(Qt::escape(reply->url().toString())).arg(Qt::escape(reply->url().toString()));
    passwordDialog.introLabel->setText(introMessage);
    passwordDialog.introLabel->setWordWrap(true);

    if (dialog.exec() == QDialog::Accepted) {
        auth->setUser(passwordDialog.userNameLineEdit->text());
        auth->setPassword(passwordDialog.passwordLineEdit->text());
    }
}

void TSNetworkAaccessManager::proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *auth)
{
    QWidget *mainWindow = TSBrowserApplication::instance()->mainWindow();

    QDialog dialog(mainWindow);
    dialog.setWindowFlags(Qt::Sheet);

    Ui::ProxyDialog proxyDialog;
    proxyDialog.setupUi(&dialog);

    proxyDialog.iconLabel->setText(QString());
    proxyDialog.iconLabel->setPixmap(mainWindow->style()->standardIcon(QStyle::SP_MessageBoxQuestion, 0, mainWindow).pixmap(32, 32));

    QString introMessage = tr("<qt>Connect to proxy \"%1\" using:</qt>");
    introMessage = introMessage.arg(Qt::escape(proxy.hostName()));
    proxyDialog.introLabel->setText(introMessage);
    proxyDialog.introLabel->setWordWrap(true);

    if (dialog.exec() == QDialog::Accepted) {
        auth->setUser(proxyDialog.userNameLineEdit->text());
        auth->setPassword(proxyDialog.passwordLineEdit->text());
    }
}

#ifndef QT_NO_OPENSSL
void TSNetworkAaccessManager::sslErrors(QNetworkReply *reply, const QList<QSslError> &error)
{
    // check if SSL certificate has been trusted already
    QString replyHost = reply->url().host() + ":" + reply->url().port();
    if(! sslTrustedHostList.contains(replyHost)) {
        QWidget *mainWindow = TSBrowserApplication::instance()->mainWindow();

        QStringList errorStrings;
        for (int i = 0; i < error.count(); ++i)
            errorStrings += error.at(i).errorString();
        QString errors = errorStrings.join(QLatin1String("\n"));
        int ret = QMessageBox::warning(mainWindow, QCoreApplication::applicationName(),
                tr("SSL Errors:\n\n%1\n\n%2\n\n"
                        "Do you want to ignore these errors for this host?").arg(reply->url().toString()).arg(errors),
                        QMessageBox::Yes | QMessageBox::No,
                        QMessageBox::No);
        if (ret == QMessageBox::Yes) {
            reply->ignoreSslErrors();
            sslTrustedHostList.append(replyHost);
        }
    }
}
#endif
