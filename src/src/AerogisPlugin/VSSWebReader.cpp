#include "VSSWebReader.h"
#include <chrono>
#include <qapplication.h>
#include <qthread.h>

QT_USE_NAMESPACE

VSSWebReader::VSSWebReader(const QUrl &url, QObject *parent)
   : QObject(parent)
   , m_url(url)
   , mDatasGot(0)
{
   connect(&m_webSocket, &QWebSocket::connected, this, &VSSWebReader::onConnected);
   connect(&m_webSocket, &QWebSocket::disconnected, this, &VSSWebReader::onClosed);
   connect(&m_webSocket, &QWebSocket::binaryMessageReceived,
      this, &VSSWebReader::onBinaryMessageReceived);
   connect(&m_webSocket, &QWebSocket::textMessageReceived,
      this, &VSSWebReader::onTextMessageReceived);
   m_webSocket.open(QUrl(url));
}

void VSSWebReader::onConnected()
{
   qDebug() << "WebSocket connected";
}

void VSSWebReader::onBinaryMessageReceived(const QByteArray &message)
{
   qDebug() << "Message received " << message.size() << " bytes\n";

   mData[mDatasGot++] = message;
   if (mDatasGot == 2)
   {
      m_webSocket.close();
   }
}

void VSSWebReader::onTextMessageReceived(const QString &message)
{
   qDebug() << "Message received " << message.size() << " symbols\n";

   mText = message;
   m_webSocket.close();
}

bool VSSWebReader::isDone()
{
   return m_webSocket.state() == QAbstractSocket::UnconnectedState;
}

bool VSSWebReader::waitDone(int sec)
{
   // get data from server
   QApplication::processEvents();
   auto start = std::chrono::system_clock::now();
   while (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count() < sec
      && !isDone())
   {
      //std::this_thread::sleep_for(std::chrono::milliseconds(500));
      QThread::usleep(500);
      QApplication::processEvents();
   }

   return isDone();
}

void VSSWebReader::onClosed()
{
   qDebug() << "WebSocket closed";
}