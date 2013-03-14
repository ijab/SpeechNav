// Copyright (C) T-Solution
//

//  
// File   : TSDownloadManager.cpp
// Author : Zhan
//


#include "TSDownloadManager.h"

#include "TSAutoSaver.h"
#include "TSBrowserApplication.h"
#include "TSNetworkAccessmanager.h"
#include "TSMainWindow.h"

#include <math.h>

#include <QtCore/QMetaEnum>
#include <QtCore/QSettings>

#include <QtGui/QDesktopServices>
#include <QtGui/QFileDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QFileIconProvider>

#include <QtCore/QDebug>

#include <QtWebKit/QWebSettings>

/*!
    TSDownloadItem is a widget that is displayed in the download manager list.
    It moves the data from the QNetworkReply into the QFile as well
    as update the information/progressbar and report errors.
 */
TSDownloadItem::TSDownloadItem(QNetworkReply *reply, bool requestFileName, QWidget *parent)
    : QWidget(parent)
    , m_reply(reply)
    , m_requestFileName(requestFileName)
    , m_bytesReceived(0)
{
    setupUi(this);
    QPalette p = downloadInfoLabel->palette();
    p.setColor(QPalette::Text, Qt::darkGray);
    downloadInfoLabel->setPalette(p);
    progressBar->setMaximum(0);
    tryAgainButton->hide();
    connect(stopButton, SIGNAL(clicked()), this, SLOT(stop()));
    connect(openButton, SIGNAL(clicked()), this, SLOT(open()));
    connect(tryAgainButton, SIGNAL(clicked()), this, SLOT(tryAgain()));

    init();
}

void TSDownloadItem::init()
{
    if (!m_reply)
        return;

    // attach to the m_reply
    m_url = m_reply->url();
    m_reply->setParent(this);
    connect(m_reply, SIGNAL(readyRead()), this, SLOT(downloadReadyRead()));
    connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(error(QNetworkReply::NetworkError)));
    connect(m_reply, SIGNAL(downloadProgress(qint64,qint64)),
            this, SLOT(downloadProgress(qint64,qint64)));
    connect(m_reply, SIGNAL(metaDataChanged()),
            this, SLOT(metaDataChanged()));
    connect(m_reply, SIGNAL(finished()),
            this, SLOT(finished()));

    // reset info
    downloadInfoLabel->clear();
    progressBar->setValue(0);
    getFileName();

    // start timer for the download estimation
    m_downloadTime.start();

    if (m_reply->error() != QNetworkReply::NoError) {
        error(m_reply->error());
        finished();
    }
}

void TSDownloadItem::getFileName()
{
    /*
	QSettings settings;
    settings.beginGroup(QLatin1String("downloadmanager"));
    QString defaultLocation = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
    QString downloadDirectory = settings.value(QLatin1String("downloadDirectory"), defaultLocation).toString();
    if (!downloadDirectory.isEmpty())
        downloadDirectory += QLatin1Char('/');

    QString defaultFileName = saveFileName(downloadDirectory);
    QString fileName = defaultFileName;
	*/
	QString defaultFileName;
	QString fileName;
    if (m_requestFileName) {
        fileName = QFileDialog::getSaveFileName(this, tr("Save File"), defaultFileName);
        if (fileName.isEmpty()) {
            m_reply->close();
            fileNameLabel->setText(tr("Download canceled: %1").arg(QFileInfo(defaultFileName).fileName()));
            return;
        }
    }
    m_output.setFileName(fileName);
    fileNameLabel->setText(QFileInfo(m_output.fileName()).fileName());
    if (m_requestFileName)
        downloadReadyRead();
}

QString TSDownloadItem::saveFileName(const QString &directory) const
{
    // Move this function into QNetworkReply to also get file name sent from the server
    QString path = m_url.path();
    QFileInfo info(path);
    QString baseName = info.completeBaseName();
    QString endName = info.suffix();

    if (baseName.isEmpty()) {
        baseName = QLatin1String("unnamed_download");
        qDebug() << "TSDownloadManager:: downloading unknown file:" << m_url;
    }
    QString name = directory + baseName + QLatin1Char('.') + endName;
    if (QFile::exists(name)) {
        // already exists, don't overwrite
        int i = 1;
        do {
            name = directory + baseName + QLatin1Char('-') + QString::number(i++) + QLatin1Char('.') + endName;
        } while (QFile::exists(name));
    }
    return name;
}


void TSDownloadItem::stop()
{
    setUpdatesEnabled(false);
    stopButton->setEnabled(false);
    stopButton->hide();
    tryAgainButton->setEnabled(true);
    tryAgainButton->show();
    setUpdatesEnabled(true);
    m_reply->abort();
}

void TSDownloadItem::open()
{
    QFileInfo info(m_output);
    QUrl url = QUrl::fromLocalFile(info.absolutePath());
    QDesktopServices::openUrl(url);
}

void TSDownloadItem::tryAgain()
{
    if (!tryAgainButton->isEnabled())
        return;

    tryAgainButton->setEnabled(false);
    tryAgainButton->setVisible(false);
    stopButton->setEnabled(true);
    stopButton->setVisible(true);
    progressBar->setVisible(true);

    QNetworkReply *r = TSBrowserApplication::networkAccessManager()->get(QNetworkRequest(m_url));
    if (m_reply)
        m_reply->deleteLater();
    if (m_output.exists())
        m_output.remove();
    m_reply = r;
    init();
    emit statusChanged();
}

void TSDownloadItem::downloadReadyRead()
{
    if (m_requestFileName && m_output.fileName().isEmpty())
        return;
    if (!m_output.isOpen()) {
        // in case someone else has already put a file there
        if (!m_requestFileName)
            getFileName();
        if (!m_output.open(QIODevice::WriteOnly)) {
            downloadInfoLabel->setText(tr("Error opening save file: %1")
                    .arg(m_output.errorString()));
            stopButton->click();
            emit statusChanged();
            return;
        }
        emit statusChanged();
    }
    if (-1 == m_output.write(m_reply->readAll())) {
        downloadInfoLabel->setText(tr("Error saving: %1")
                .arg(m_output.errorString()));
        stopButton->click();
    }
}

void TSDownloadItem::error(QNetworkReply::NetworkError)
{
    qDebug() << "TSDownloadItem::error" << m_reply->errorString() << m_url;
    downloadInfoLabel->setText(tr("Network Error: %1").arg(m_reply->errorString()));
    tryAgainButton->setEnabled(true);
    tryAgainButton->setVisible(true);
}

void TSDownloadItem::metaDataChanged()
{
    qDebug() << "TSDownloadItem::metaDataChanged: not handled.";
}

void TSDownloadItem::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    m_bytesReceived = bytesReceived;
    if (bytesTotal == -1) {
        progressBar->setValue(0);
        progressBar->setMaximum(0);
    } else {
        progressBar->setValue(bytesReceived);
        progressBar->setMaximum(bytesTotal);
    }
    updateInfoLabel();
}

void TSDownloadItem::updateInfoLabel()
{
    if (m_reply->error() == QNetworkReply::NoError)
        return;

    qint64 bytesTotal = progressBar->maximum();
    bool running = !downloadedSuccessfully();

    // update info label
    double speed = m_bytesReceived * 1000.0 / m_downloadTime.elapsed();
    double timeRemaining = ((double)(bytesTotal - m_bytesReceived)) / speed;
    QString timeRemainingString = tr("seconds");
    if (timeRemaining > 60) {
        timeRemaining = timeRemaining / 60;
        timeRemainingString = tr("minutes");
    }
    timeRemaining = floor(timeRemaining);

    // When downloading the eta should never be 0
    if (timeRemaining == 0)
        timeRemaining = 1;

    QString info;
    if (running) {
        QString remaining;
        if (bytesTotal != 0)
            remaining = tr("- %4 %5 remaining")
            .arg(timeRemaining)
            .arg(timeRemainingString);
        info = QString(tr("%1 of %2 (%3/sec) %4"))
            .arg(dataString(m_bytesReceived))
            .arg(bytesTotal == 0 ? tr("?") : dataString(bytesTotal))
            .arg(dataString((int)speed))
            .arg(remaining);
    } else {
        if (m_bytesReceived == bytesTotal)
            info = dataString(m_output.size());
        else
            info = tr("%1 of %2 - Stopped")
                .arg(dataString(m_bytesReceived))
                .arg(dataString(bytesTotal));
    }
    downloadInfoLabel->setText(info);
}

QString TSDownloadItem::dataString(int size) const
{
    QString unit;
    if (size < 1024) {
        unit = tr("bytes");
    } else if (size < 1024*1024) {
        size /= 1024;
        unit = tr("kB");
    } else {
        size /= 1024*1024;
        unit = tr("MB");
    }
    return QString(QLatin1String("%1 %2")).arg(size).arg(unit);
}

bool TSDownloadItem::downloading() const
{
    return (progressBar->isVisible());
}

bool TSDownloadItem::downloadedSuccessfully() const
{
    return (stopButton->isHidden() && tryAgainButton->isHidden());
}

void TSDownloadItem::finished()
{
    progressBar->hide();
    stopButton->setEnabled(false);
    stopButton->hide();
    m_output.close();
    updateInfoLabel();
    emit statusChanged();
}

/*!
    TSDownloadManager is a Dialog that contains a list of DownloadItems

    It is a basic download manager.  It only downloads the file, doesn't do BitTorrent,
    extract zipped files or anything fancy.
  */
TSDownloadManager::TSDownloadManager(QWidget *parent)
    : QWidget(parent)
    , m_autoSaver(new TSAutoSaver(this))
    , m_manager(TSBrowserApplication::networkAccessManager())
    , m_iconProvider(0)
    , m_removePolicy(Never)
{
    setupUi(this);
    downloadsView->setShowGrid(false);
    downloadsView->verticalHeader()->hide();
    downloadsView->horizontalHeader()->hide();
    downloadsView->setAlternatingRowColors(true);
    downloadsView->horizontalHeader()->setStretchLastSection(true);
    m_model = new TSDownloadModel(this);
    downloadsView->setModel(m_model);
    connect(cleanupButton, SIGNAL(clicked()), this, SLOT(cleanup()));
    load();
}

TSDownloadManager::~TSDownloadManager()
{
    m_autoSaver->changeOccurred();
    m_autoSaver->saveIfNeccessary();
    if (m_iconProvider)
        delete m_iconProvider;
}

int TSDownloadManager::activeDownloads() const
{
    int count = 0;
    for (int i = 0; i < m_downloads.count(); ++i) {
        if (m_downloads.at(i)->stopButton->isEnabled())
            ++count;
    }
    return count;
}

void TSDownloadManager::download(const QNetworkRequest &request, bool requestFileName)
{
    if (request.url().isEmpty())
        return;
    handleUnsupportedContent(m_manager->get(request), requestFileName);
}

void TSDownloadManager::handleUnsupportedContent(QNetworkReply *reply, bool requestFileName)
{
    if (!reply || reply->url().isEmpty())
        return;
    QVariant header = reply->header(QNetworkRequest::ContentLengthHeader);
    bool ok;
    int size = header.toInt(&ok);
    if (ok && size == 0)
        return;

    qDebug() << "TSDownloadManager::handleUnsupportedContent" << reply->url() << "requestFileName" << requestFileName;
    TSDownloadItem *item = new TSDownloadItem(reply, requestFileName, this);
    addItem(item);

	// Show download manager dock widget
	TSMainWindow *mw = dynamic_cast<TSMainWindow*>(TSBrowserApplication::instance()->mainWindow());
	if(mw) mw->showDownloadManager(this);
}

void TSDownloadManager::addItem(TSDownloadItem *item)
{
    connect(item, SIGNAL(statusChanged()), this, SLOT(updateRow()));
    int row = m_downloads.count();
    m_model->beginInsertRows(QModelIndex(), row, row);
    m_downloads.append(item);
    m_model->endInsertRows();
    updateItemCount();
    if (row == 0)
        show();
    downloadsView->setIndexWidget(m_model->index(row, 0), item);
    QIcon icon = style()->standardIcon(QStyle::SP_FileIcon);
    item->fileIcon->setPixmap(icon.pixmap(48, 48));
    downloadsView->setRowHeight(row, item->sizeHint().height());
}

void TSDownloadManager::updateRow()
{
    TSDownloadItem *item = qobject_cast<TSDownloadItem*>(sender());
    int row = m_downloads.indexOf(item);
    if (-1 == row)
        return;
    if (!m_iconProvider)
        m_iconProvider = new QFileIconProvider();
    QIcon icon = m_iconProvider->icon(item->m_output.fileName());
    if (icon.isNull())
        icon = style()->standardIcon(QStyle::SP_FileIcon);
    item->fileIcon->setPixmap(icon.pixmap(48, 48));
    downloadsView->setRowHeight(row, item->minimumSizeHint().height());

    bool remove = false;
    QWebSettings *globalSettings = QWebSettings::globalSettings();
    if (!item->downloading()
        && globalSettings->testAttribute(QWebSettings::PrivateBrowsingEnabled))
        remove = true;

    if (item->downloadedSuccessfully()
        && removePolicy() == TSDownloadManager::SuccessFullDownload) {
        remove = true;
    }
    if (remove)
        m_model->removeRow(row);

    cleanupButton->setEnabled(m_downloads.count() - activeDownloads() > 0);
}

TSDownloadManager::RemovePolicy TSDownloadManager::removePolicy() const
{
    return m_removePolicy;
}

void TSDownloadManager::setRemovePolicy(RemovePolicy policy)
{
    if (policy == m_removePolicy)
        return;
    m_removePolicy = policy;
    m_autoSaver->changeOccurred();
}

void TSDownloadManager::save() const
{
    QSettings settings;
    settings.beginGroup(QLatin1String("downloadmanager"));
    QMetaEnum removePolicyEnum = staticMetaObject.enumerator(staticMetaObject.indexOfEnumerator("RemovePolicy"));
    settings.setValue(QLatin1String("removeDownloadsPolicy"), QLatin1String(removePolicyEnum.valueToKey(m_removePolicy)));
    settings.setValue(QLatin1String("size"), size());
    if (m_removePolicy == Exit)
        return;

    for (int i = 0; i < m_downloads.count(); ++i) {
        QString key = QString(QLatin1String("download_%1_")).arg(i);
        settings.setValue(key + QLatin1String("url"), m_downloads[i]->m_url);
        settings.setValue(key + QLatin1String("location"), QFileInfo(m_downloads[i]->m_output).filePath());
        settings.setValue(key + QLatin1String("done"), m_downloads[i]->downloadedSuccessfully());
    }
    int i = m_downloads.count();
    QString key = QString(QLatin1String("download_%1_")).arg(i);
    while (settings.contains(key + QLatin1String("url"))) {
        settings.remove(key + QLatin1String("url"));
        settings.remove(key + QLatin1String("location"));
        settings.remove(key + QLatin1String("done"));
        key = QString(QLatin1String("download_%1_")).arg(++i);
    }
}

void TSDownloadManager::load()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("downloadmanager"));
    QSize size = settings.value(QLatin1String("size")).toSize();
    if (size.isValid())
        resize(size);
    QByteArray value = settings.value(QLatin1String("removeDownloadsPolicy"), QLatin1String("Never")).toByteArray();
    QMetaEnum removePolicyEnum = staticMetaObject.enumerator(staticMetaObject.indexOfEnumerator("RemovePolicy"));
    m_removePolicy = removePolicyEnum.keyToValue(value) == -1 ?
                        Never :
                        static_cast<RemovePolicy>(removePolicyEnum.keyToValue(value));

    int i = 0;
    QString key = QString(QLatin1String("download_%1_")).arg(i);
    while (settings.contains(key + QLatin1String("url"))) {
        QUrl url = settings.value(key + QLatin1String("url")).toUrl();
        QString fileName = settings.value(key + QLatin1String("location")).toString();
        bool done = settings.value(key + QLatin1String("done"), true).toBool();
        if (!url.isEmpty() && !fileName.isEmpty()) {
            TSDownloadItem *item = new TSDownloadItem(0, this);
            item->m_output.setFileName(fileName);
            item->fileNameLabel->setText(QFileInfo(item->m_output.fileName()).fileName());
            item->m_url = url;
            item->stopButton->setVisible(false);
            item->stopButton->setEnabled(false);
            item->tryAgainButton->setVisible(!done);
            item->tryAgainButton->setEnabled(!done);
            item->progressBar->setVisible(!done);
            addItem(item);
        }
        key = QString(QLatin1String("download_%1_")).arg(++i);
    }
    cleanupButton->setEnabled(m_downloads.count() - activeDownloads() > 0);
}

void TSDownloadManager::cleanup()
{
    if (m_downloads.isEmpty())
        return;
    m_model->removeRows(0, m_downloads.count());
    updateItemCount();
    if (m_downloads.isEmpty() && m_iconProvider) {
        delete m_iconProvider;
        m_iconProvider = 0;
    }
    m_autoSaver->changeOccurred();
}

void TSDownloadManager::updateItemCount()
{
    int count = m_downloads.count();
    itemCount->setText(count == 1 ? tr("1 Download") : tr("%1 Downloads").arg(count));
}

TSDownloadModel::TSDownloadModel(TSDownloadManager *downloadManager, QObject *parent)
    : QAbstractListModel(parent)
    , m_downloadManager(downloadManager)
{
}

QVariant TSDownloadModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= rowCount(index.parent()))
        return QVariant();
    if (role == Qt::ToolTipRole)
        if (!m_downloadManager->m_downloads.at(index.row())->downloadedSuccessfully())
            return m_downloadManager->m_downloads.at(index.row())->downloadInfoLabel->text();
    return QVariant();
}

int TSDownloadModel::rowCount(const QModelIndex &parent) const
{
    return (parent.isValid()) ? 0 : m_downloadManager->m_downloads.count();
}

bool TSDownloadModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (parent.isValid())
        return false;

    int lastRow = row + count - 1;
    for (int i = lastRow; i >= row; --i) {
        if (m_downloadManager->m_downloads.at(i)->downloadedSuccessfully()
            || m_downloadManager->m_downloads.at(i)->tryAgainButton->isEnabled()) {
            beginRemoveRows(parent, i, i);
            m_downloadManager->m_downloads.takeAt(i)->deleteLater();
            endRemoveRows();
        }
    }
    m_downloadManager->m_autoSaver->changeOccurred();
    return true;
}

