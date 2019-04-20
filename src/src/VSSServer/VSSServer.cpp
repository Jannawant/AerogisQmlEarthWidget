#include "VSSServer.h"
#include "QtWebSockets/QWebSocketServer"
#include "QtWebSockets/QWebSocket"
#include <QtCore/QDebug>
#include <iostream>
#include <assert.h>
#include <qfile.h>
#include <qthread.h>
#include <qnetworkinterface.h>

QT_USE_NAMESPACE

//! [constructor]
VSSServer::VSSServer(quint16 port, QString confName, QString earthName, QByteArray &buf1, QByteArray &buf2, QMutex &mut, bool &ok, QObject *parent)
   : QObject(parent)
   , m_pWebSocketServer(Q_NULLPTR)
   , mArchServer(Q_NULLPTR)
   , mConfigServer(Q_NULLPTR)
   , mConfName(confName)
   , mEarthName(earthName)
   , mBuf1(buf1)
   , mBuf2(buf2)
   , mMut(mut)
{
   m_pWebSocketServer = new QWebSocketServer(QStringLiteral("VSS Server"),
      QWebSocketServer::NonSecureMode,
      this);
   mArchServer = new QWebSocketServer(QStringLiteral("VSS Archive Server"),
      QWebSocketServer::NonSecureMode,
      this);
   mConfigServer = new QWebSocketServer(QStringLiteral("VSS Config Server"),
      QWebSocketServer::NonSecureMode,
      this);
   mEarthServer = new QWebSocketServer(QStringLiteral("VSS Earth Server"),
      QWebSocketServer::NonSecureMode,
      this);

   bool ok1 = m_pWebSocketServer->listen(QHostAddress::Any, port);
   bool ok2 = mArchServer->listen(QHostAddress::Any, port + 1);
   bool ok3 = mConfigServer->listen(QHostAddress::Any, port + 2);
   bool ok4 = mEarthServer->listen(QHostAddress::Any, port + 3);

   if (ok = (ok1 && ok2 && ok3 && ok4))
   {
      QString addr;
      for(const QHostAddress &address : QNetworkInterface::allAddresses())
      {
         if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
            addr += address.toString() + " ";
      }

      qDebug() << "VSS Server "<< addr.toStdString().c_str() << "listening ports " << port << "-" << (port + 3);
      connect(m_pWebSocketServer, &QWebSocketServer::newConnection,
         this, &VSSServer::onNewConnection);
      qDebug() << "data (coordinates) on port " << port;
      connect(mArchServer, &QWebSocketServer::newConnection,
         this, &VSSServer::onNewConnectionArch);
      qDebug() << "flight archive on port " << (port+1);
      connect(mConfigServer, &QWebSocketServer::newConnection,
         this, &VSSServer::onNewConnectionConfig);
      qDebug() << "config file on port " << (port + 2);
      connect(mEarthServer, &QWebSocketServer::newConnection,
         this, &VSSServer::onNewConnectionEarth);
      qDebug() << "earth file on port " << (port + 3) << "\n";
   }
   else
   {
      std::cerr << "error: cannot start WebSocket Server on ports " << port <<"-"<<(port+3)<< "\n";
   }
}

VSSServer::~VSSServer()
{
   if(m_pWebSocketServer) m_pWebSocketServer->close();
   if (mArchServer) mArchServer->close();
   if (mConfigServer) mConfigServer->close();
   if (mEarthServer) mEarthServer->close();
   qDeleteAll(oldSock);
}
//! [constructor]

//! [onNewConnection]
void VSSServer::onNewConnection()
{
   QWebSocket *pSocket = m_pWebSocketServer->nextPendingConnection();

   QByteArray mLocBuf1, mLocBuf2;
   {
      // get fresh data
      QMutexLocker ml(&mMut);
      mLocBuf1 = mBuf1;
      mLocBuf2 = mBuf2;
      storeSock(pSocket);
   }
   // send data
   pSocket->sendBinaryMessage(mLocBuf1);
   pSocket->sendBinaryMessage(mLocBuf2);
   pSocket->flush();
   std::cout << "data sent, " << mLocBuf1.size() << " and " << mLocBuf2.size() << " bytes\n";
}

void VSSServer::onNewConnectionArch()
{
   QWebSocket *pSocket = mArchServer->nextPendingConnection();
   pSocket->flush();

   QVector<QByteArray> locArch;
   {
      // get fresh data
      QMutexLocker ml(&mMut);
      locArch = mArchive;
      storeSock( pSocket );
   }
   // send data
   QByteArray sizes, datas;
   auto pushI = [&sizes](int v)
   {
      sizes.append((const char*)(&v),4);
   };
   pushI(locArch.size());
   for (auto &ba : locArch)
   {
      pushI(ba.size());
      datas.append(ba);
   }
   assert(sizes.size() == (locArch.size() + 1) * 4);

   pSocket->sendBinaryMessage(sizes);
   pSocket->sendBinaryMessage(datas);
   pSocket->flush();
   std::cout << "Archive Path data sent, " << sizes.size() << " and " << datas.size() << " bytes\n";
}

void VSSServer::onNewConnectionConfig()
{
   QWebSocket *pSocket = mConfigServer->nextPendingConnection();

   // read file
   QFile f(mConfName);
   QString data;
   if (f.open(QFile::ReadOnly | QFile::Text))
   {
      QTextStream in(&f);
      data = in.readAll();
   }

   // send data
   if (!data.isEmpty())
   {
      pSocket->sendTextMessage(data);
      pSocket->flush();
      std::cout << "Config sent, " << data.size() << " symbols\n";
   }
   else
   {
      std::cout << "no config file to send, or could not read " << mConfName.toStdString() << "\n";
   }

   //pSocket->close(QWebSocketProtocol::CloseCodeNormal, "done");
   //delete pSocket;
   {
      QMutexLocker ml(&mMut);
      storeSock(pSocket);
   }
}

void VSSServer::onNewConnectionEarth()
{
   QWebSocket *pSocket = mEarthServer->nextPendingConnection();

   // read file
   QFile f(mEarthName);
   QString data;
   if (f.open(QFile::ReadOnly | QFile::Text))
   {
      QTextStream in(&f);
      data = in.readAll();
   }

   // send data
   if (!data.isEmpty())
   {
      pSocket->sendTextMessage(data);
      pSocket->flush();
      std::cout << "Earth file '" << mEarthName.toStdString() << "' sent, " << data.size() << " symbols\n";
   }
   else
   {
      std::cout << "no earth file '" << mEarthName.toStdString() << "' to send, or could not read " << mEarthName.toStdString() << "\n";
   }

   {
      QMutexLocker ml(&mMut);
      storeSock(pSocket);
   }
}


//! [onNewConnection]

void VSSServer::onDataReceived()
{
   // get fresh data
   QMutexLocker ml(&mMut);
   mArchive << mBuf1 << mBuf2;
   int sz = mArchive.size();
   assert(sz % 2 == 0);
   assert(sz <= 200);
   if (sz == 200)
   {
      // 200 datas -> 100 points
      // remove (half - 1) = 49 points (i.e. leave 102 datas)
      int sz2 = sz / 2;
      for (int i = 2; i < sz2; i+=2)
      {
         mArchive.removeAt(i);
         mArchive.removeAt(i);
      }
   }
}

void VSSServer::storeSock(QWebSocket *ws)
{
   oldSock << ws;
   if (oldSock.size() > 1000)
   {
      oldSock.first()->deleteLater();
      oldSock.removeFirst();
   }
}

