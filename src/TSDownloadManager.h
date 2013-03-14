// Copyright (C) T-Solution
//

//  
// File   : TSDownloadManager.h
// Author : Zhan
//

#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include "ui_downloads.h"
#include "ui_downloaditem.h"

#include <QtNetwork/QNetworkReply>

#include <QtCore/QFile>
#include <QtCore/QTime>

class TSDownloadItem : public QWidget, public Ui_TSDownloadItem
{
    Q_OBJECT

signals:
    void statusChanged();

public:
    TSDownloadItem(QNetworkReply *reply = 0, bool requestFileName = true, QWidget *parent = 0);
    bool downloading() const;
    bool downloadedSuccessfully() const;

    QUrl m_url;

    QFile m_output;
    QNetworkReply *m_reply;

private slots:
    void stop();
    void tryAgain();
    void open();

    void downloadReadyRead();
    void error(QNetworkReply::NetworkError code);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void metaDataChanged();
    void finished();

private:
    void getFileName();
    void init();
    void updateInfoLabel();
    QString dataString(int size) const;

    QString saveFileName(const QString &directory) const;

    bool m_requestFileName;
    qint64 m_bytesReceived;
    QTime m_downloadTime;
};

class TSAutoSaver;
class TSDownloadModel;
QT_BEGIN_NAMESPACE
class QFileIconProvider;
QT_END_NAMESPACE

class TSDownloadManager : public QWidget, public Ui_TSDownloadWidget
{
    Q_OBJECT
    Q_PROPERTY(RemovePolicy removePolicy READ removePolicy WRITE setRemovePolicy)
    Q_ENUMS(RemovePolicy)

public:
    enum RemovePolicy {
        Never,
        Exit,
        SuccessFullDownload
    };

    TSDownloadManager(QWidget *parent = 0);
    ~TSDownloadManager();
    int activeDownloads() const;

    RemovePolicy removePolicy() const;
    void setRemovePolicy(RemovePolicy policy);

public slots:
    void download(const QNetworkRequest &request, bool requestFileName = true);
    inline void download(const QUrl &url, bool requestFileName = true)
        { download(QNetworkRequest(url), requestFileName); }
    void handleUnsupportedContent(QNetworkReply *reply, bool requestFileName = true);
    void cleanup();

private slots:
    void save() const;
    void updateRow();

private:
    void addItem(TSDownloadItem *item);
    void updateItemCount();
    void load();

    TSAutoSaver *m_autoSaver;
    TSDownloadModel *m_model;
    QNetworkAccessManager *m_manager;
    QFileIconProvider *m_iconProvider;
    QList<TSDownloadItem*> m_downloads;
    RemovePolicy m_removePolicy;
    friend class TSDownloadModel;
};

class TSDownloadModel : public QAbstractListModel
{
    friend class TSDownloadManager;
    Q_OBJECT

public:
    TSDownloadModel(TSDownloadManager *downloadManager, QObject *parent = 0);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

private:
    TSDownloadManager *m_downloadManager;

};

#endif // DOWNLOADMANAGER_H

