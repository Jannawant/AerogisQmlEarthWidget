#ifndef VSSSERVER_H
#define VSSSERVER_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QByteArray>
#include <QtCore/qvector.h>

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)

class VSSServer : public QObject
{
   Q_OBJECT;
public:
   explicit VSSServer(quint16 port, QString confName, QString earthName, QByteArray &buf1, QByteArray &buf2, QMutex &mut, bool &ok, QObject *parent = Q_NULLPTR);
   virtual ~VSSServer();

public slots:
   void onDataReceived();

protected Q_SLOTS:
   void onNewConnection();
   void onNewConnectionArch();
   void onNewConnectionConfig();
   void onNewConnectionEarth();

protected:
   QWebSocketServer *m_pWebSocketServer, *mArchServer, *mConfigServer, *mEarthServer;
   QList<QWebSocket*> oldSock;

   QByteArray &mBuf1, &mBuf2;
   QMutex &mMut;

   QVector<QByteArray> mArchive;
   QString mConfName, mEarthName;

   void storeSock(QWebSocket *ws);
};

#endif //VSSSERVER_H
