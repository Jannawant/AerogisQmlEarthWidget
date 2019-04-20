#include "VSSComReader.h"
#include <QtSerialPort/qserialport.h>
#include <QtSerialPort/qserialportinfo.h>
#include <iostream>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include "../AerogisPlugin/VSSProtocol.h"

#define VSS_DATA_TRANSFER_TIME_MS 1500

VSSComReader::VSSComReader(QString COMName, QObject *parent, bool activateTimer)
   : QObject(parent)
{
   mSerialPort.setPortName(COMName);

   if (activateTimer)
   {
      mTimer = new QTimer(this);
      connect(mTimer, SIGNAL(timeout()), this, SLOT(readData()));
      mTimer->start(5000);
   }
}

VSSComReader::~VSSComReader()
{
   if (mSerialPort.openMode() != QIODevice::NotOpen)
      mSerialPort.close();
}

bool VSSComReader::OpenDevice(QString COMName)
{
   if (mSerialPort.openMode() != QIODevice::NotOpen)
      mSerialPort.close();

   // "BAUD=19200 PARITY=N DATA=8 STOP=1 XON=OFF ODSR=OFF OCTS=OFF DTR=ON RTS=OFF IDSR=OFF"
   mSerialPort.setPortName(COMName);

   mSerialPort.setBaudRate(19200);
   mSerialPort.setParity(QSerialPort::NoParity);
   mSerialPort.setDataBits(QSerialPort::Data8);
   mSerialPort.setStopBits(QSerialPort::OneStop);
   mSerialPort.setFlowControl(QSerialPort::NoFlowControl);
   mSerialPort.setReadBufferSize(1048576);
   // skip ODSR OCTS

   if (!mSerialPort.open(QIODevice::ReadWrite))
   {
      QSerialPort::SerialPortError err = mSerialPort.error();
      QString ErrorMessage = "err_CANNOT_OPEN_COM " + mSerialPort.portName() + ": " + QString::number(err) + "\n available ports are: ";

      auto lst = QSerialPortInfo::availablePorts();
      for (auto &p : lst)
      {
         QString nm = p.portName();
         ErrorMessage += nm + " ";
      }

      std::cerr << ErrorMessage.toStdString() << "\n";
      return false;
   }

   mSerialPort.setDataTerminalReady(true);
   mSerialPort.setRequestToSend(false);
   // skip IDSR

   return true;
}

void VSSComReader::readData()
{
   readData(mSerialPort.portName());
}

bool VSSComReader::readData(QString COMName)
{
   if (!OpenDevice(COMName))
      return false;

   // free COM port buffer (read old data)
   DWORD Count1 = 0;
   while ((Count1 = mSerialPort.bytesAvailable()) > 0)
   {
      QByteArray ba = mSerialPort.read(Count1);
   }

   QList<QByteArray> datas;

   //Выставляем флаг готовности приема данных с ВСС
   for (TVSSID VSSID : {ID_VSS1, ID_VSS2})
   {
      if (mSerialPort.write((char*)&VSSID, 1) == 1)
      {
         mSerialPort.flush();

         auto start = std::chrono::system_clock::now();
         while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count() < VSS_DATA_TRANSFER_TIME_MS)
         {
            mSerialPort.waitForReadyRead(200);
         }

         //Получаем данные
         DWORD Count2 = mSerialPort.bytesAvailable();
         if (Count2 > 0)
         {
            datas << mSerialPort.read(Count2);
         }
      }
   }

   int readSz = 0;
   for (auto &ba : datas)
   {
      readSz += ba.size();
   }

   std::cout << "read " << readSz << " bytes from " << mSerialPort.portName().toStdString() <<  "\n";
   if (datas.size() == 2)
   {
      {
         QMutexLocker ml(&mMut);
         mBuf1 = datas[0];
         mBuf2 = datas[1];
         // avoid dead lock! (by trying to lock in the same thread)
      }
      emit dataReceived();
      return true;
   }
   else
   {
      return false;
   }
}