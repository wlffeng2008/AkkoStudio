#include "Downloader.h"

#include <QDebug>

Downloader::Downloader(QObject *parent)
    : QObject(parent),
    m_manager(new QNetworkAccessManager(this)),
    m_reply(nullptr),
    m_downloadedSize(0),
    m_isPaused(false),
    m_isCanceled(false)
{
}

Downloader::~Downloader()
{
    cancel();
}

void Downloader::startDownload(const QString &url, const QString &savePath)
{
    cancel();
    m_url = url;
    m_downloadedSize = 0;
    m_isPaused = false;
    m_isCanceled = false;

    if(!savePath.isEmpty())
    {
        m_file.setFileName(savePath);
        // 文件不存在新建，存在则追加（断点）
        if (!m_file.open(QIODevice::ReadWrite | QIODevice::Append))
        {
            emit errorOccurred("无法打开文件：" + m_file.errorString());
            //return;
        }
    }
    if(m_file.isOpen())
        m_downloadedSize = m_file.size();
    resume();
}

void Downloader::pause()
{
    if (m_reply)
    {
        m_reply->abort();
        m_reply->deleteLater();
        m_reply = nullptr;
    }
    m_isPaused = true;
}

void Downloader::resume()
{
    if (m_isCanceled) return;
    //if (!m_file.isOpen()) return;

    m_isPaused = false;
    QNetworkRequest req((QUrl(m_url)));

    // 断点核心：Range 请求头
    if (m_downloadedSize > 0)
    {
        QString range = QString("bytes=%1-").arg(m_downloadedSize);
        req.setRawHeader("Range", range.toUtf8());
    }

    m_reply = m_manager->get(req);
    connect(m_reply, &QNetworkReply::readyRead, this, &Downloader::onReadyRead);
    connect(m_reply, &QNetworkReply::downloadProgress, this, &Downloader::downloadProgress);
    connect(m_reply, &QNetworkReply::finished, this, &Downloader::onReplyFinished);
}

void Downloader::cancel()
{
    m_isCanceled = true;
    m_isPaused = false;
    if (m_reply)
    {
        m_reply->abort();
        m_reply->deleteLater();
        m_reply = nullptr;
    }
    if (m_file.isOpen())
        m_file.close();
    m_downloadedSize = 0;
}

void Downloader::onReadyRead()
{
    if (!m_reply || m_isPaused || m_isCanceled)
        return;
    QByteArray read=m_reply->readAll();
    emit dataIn(read);
    if(m_file.isOpen())
        m_file.write(read);
}

void Downloader::downloadProgress(qint64 recv, qint64 total)
{
    if (total <= 0) return;
    qint64 currTotal = m_downloadedSize + recv;
    int percent = static_cast<int>(currTotal * 100 / total);
    emit progressChanged(percent);
}

void Downloader::onReplyFinished()
{
    if (!m_reply) return;
    QNetworkReply *reply = m_reply;
    m_reply = nullptr;

    // 写入剩余缓存数据
    if (reply->error() == QNetworkReply::NoError)
    {
        if(m_file.isOpen())
        {
            m_file.write(reply->readAll());
            m_file.close();
        }
        emit finished(true, "下载完成");
    }
    else
    {
        // 主动暂停不算错误
        if (reply->error() == QNetworkReply::OperationCanceledError && m_isPaused)
        {
            emit finished(false, "已暂停");
        }
        else if (m_isCanceled)
        {
            m_file.close();
            emit finished(false, "已取消");
        }
        else
        {
            QString err = reply->errorString();
            emit errorOccurred("下载失败：" + err);
        }
    }
    reply->deleteLater();
}