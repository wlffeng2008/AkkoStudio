#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QUrl>

class Downloader : public QObject
{
    Q_OBJECT
public:
    explicit Downloader(QObject *parent = nullptr);
    ~Downloader();
\
    void startDownload(const QString &url, const QString &savePath);
    void pause();
    void resume();
    void cancel();

signals:
    void progressChanged(int percent);
    void dataIn(const QByteArray&data);
    void finished(bool success, const QString &msg);
    void errorOccurred(const QString &errMsg);

private slots:
    void onReadyRead();
    void onReplyFinished();
    void downloadProgress(qint64 recv, qint64 total);

private:
    QNetworkAccessManager *m_manager=nullptr;
    QNetworkReply *m_reply=nullptr;
    QFile m_file;
    QString m_url;
    qint64 m_downloadedSize; // 已下载字节
    bool m_isPaused;
    bool m_isCanceled;
};

#endif // DOWNLOADER_H
