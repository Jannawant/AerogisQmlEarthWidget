#ifndef VSSWEBREADER_H
#define VSSWEBREADER_H

#include <QObject>

#include <QtCore/QObject>
#include <QtWebSockets/QWebSocket>
#include <atomic>

/** Web reader. Reads 2 blob or 1 text message, and closes */
class VSSWebReader : public QObject
{
   Q_OBJECT
public:
   explicit VSSWebReader(const QUrl &url, QObject *parent = Q_NULLPTR);

   std::atomic<int> mDatasGot;
   QByteArray mData[2];
   QString mText;

   bool isDone();
   bool waitDone(int sec);

private Q_SLOTS:
   void onConnected();
   void onBinaryMessageReceived(const QByteArray &message);
   void onTextMessageReceived(const QString &message);
   void onClosed();

private:
   QWebSocket m_webSocket;
   QUrl m_url;
};
#endif // VSSWEBREADER_H
