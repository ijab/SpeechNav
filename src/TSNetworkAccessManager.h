// Copyright (C) T-Solution
//

//  
// File   : TSNetworkAccessManager.h
// Author : Zhan
//


#ifndef NETWORKACCESSMANAGER_H
#define NETWORKACCESSMANAGER_H

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>

class TSNetworkAaccessManager : public QNetworkAccessManager
{
    Q_OBJECT

public:
    TSNetworkAaccessManager(QObject *parent = 0);

    virtual QNetworkReply* createRequest ( Operation op, const QNetworkRequest & req, QIODevice * outgoingData = 0 );

private:
    QList<QString> sslTrustedHostList;
    qint64 requestFinishedCount;
    qint64 requestFinishedFromCacheCount;
    qint64 requestFinishedPipelinedCount;
    qint64 requestFinishedSecureCount;

public slots:
    void loadSettings();
    void requestFinished(QNetworkReply *reply);

private slots:
    void authenticationRequired(QNetworkReply *reply, QAuthenticator *auth);
    void proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *auth);
#ifndef QT_NO_OPENSSL
    void sslErrors(QNetworkReply *reply, const QList<QSslError> &error);
#endif
};

#endif // NETWORKACCESSMANAGER_H
