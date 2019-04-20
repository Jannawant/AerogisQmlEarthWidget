#ifndef VSSCOMREADER_H
#define VSSCOMREADER_H

#include <QObject>
#include <list>
#include <QtSerialPort/QSerialPort>
#include <qtimer.h>

//-------------------------------------------------------------------------------
// 1. construct
// 2. readData();

class VSSComReader : public QObject
{
   Q_OBJECT;

public:
   VSSComReader(QString COMName, QObject *parent, bool activateTimer);
   ~VSSComReader();

   QByteArray mBuf1, mBuf2;
   QMutex mMut;
    
public slots:
   bool readData(QString COMName);
   void readData();

signals:
   void dataReceived();

protected:
   bool OpenDevice(QString COMName); // true if opened ok

   QSerialPort mSerialPort;

   QTimer *mTimer;
};

#endif // VSSCOMREADER_H
