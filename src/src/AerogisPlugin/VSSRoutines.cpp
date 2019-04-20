
#include "VSSRoutines.h"
#include "VSSProtocol.h"
#include "bit.h"
#include <iostream>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include <osg/Notify>
#include "QtWebSockets/QWebSocketServer"
#include "QtWebSockets/QWebSocket"
#include "VSSWebReader.h"
#include <qapplication.h>
#include <qthread.h>

#ifdef _MSC_VER
#include "VSSComReader.h"
#endif   // Windows only


#define SAFE_DELETE_ARRAY(p) if(p) delete [] p
#define BIRS_THROW throw
#define VSS_DATA_TRANSFER_TIME_MS 1500

#ifndef _T
#define _T
#endif


namespace sys
{
   void log(const char*s)
   {
      printf(s);
   }

   void log(const char*s, int n)
   {
      printf(s, n);
   }
}


//-------------------------------------------------------------------------------
void VSSRoutines::parseVSSData(std::list<CVSSData>& _VSSDataPackets, const QByteArray &buf, int &params_count, int &packets_count)
{
   DWORD Count2 = buf.size();
   //Обрабатываем данные
   //Создаем два массива, в одном храним данные, в другом храним последовательность
   //поступления данных различного типа, потом "расфысовываем" их по своим пакетам.
   //!!! данные с ВСС могут идти в любом порядке - задача - сформировать пакеты !!!
   //максимум - все пакеты одного типа !!!
   int VSSDataPacketsMaxSize = (int)ceil((double)Count2 / (double)4);
   CVSSData* VSSDataPackets = new CVSSData[VSSDataPacketsMaxSize];
   int* VSSPacketsNumbers = new int[CVSSData::VSSParametersCount];
   for (int n1 = 0; n1 < CVSSData::VSSParametersCount; n1++)
      VSSPacketsNumbers[n1] = 0;

   for (uint i = 0; i < Count2; i += 4) //параметр ВСС - 4 байта !
   {
      params_count++;

      CVSSData VSSData;
      DWORD VSSParameter = bit::CreateDWORD(buf[i], buf[i + 1], buf[i + 2], buf[i + 3]);
      CVSSProtocol::DecodeParameter(VSSParameter, &VSSData);

      TVSSParameterType vt = CVSSProtocol::DecodeParameterType(VSSParameter);
      switch (vt)
      {
      case vss_pt_Longitude:
         VSSDataPackets[VSSPacketsNumbers[0]].m_LongitudeIsValid = VSSData.m_LongitudeIsValid;
         VSSDataPackets[VSSPacketsNumbers[0]].m_Longitude = VSSData.m_Longitude;
         VSSDataPackets[VSSPacketsNumbers[0]].m_LongitudeDirection = VSSData.m_LongitudeDirection;
         VSSDataPackets[VSSPacketsNumbers[0]].m_ValidParametersCount++;
         VSSPacketsNumbers[0]++;
         break;

      case vss_pt_Latitude:
         VSSDataPackets[VSSPacketsNumbers[1]].m_LatitudeIsValid = VSSData.m_LatitudeIsValid;
         VSSDataPackets[VSSPacketsNumbers[1]].m_Latitude = VSSData.m_Latitude;
         VSSDataPackets[VSSPacketsNumbers[1]].m_LatitudeDirection = VSSData.m_LatitudeDirection;
         VSSDataPackets[VSSPacketsNumbers[1]].m_ValidParametersCount++;
         VSSPacketsNumbers[1]++;
         break;

      case vss_pt_LeftDistanceMile:
         VSSDataPackets[VSSPacketsNumbers[2]].m_LeftDistanceMileIsValid = VSSData.m_LeftDistanceMileIsValid;
         VSSDataPackets[VSSPacketsNumbers[2]].m_LeftDistanceMile = VSSData.m_LeftDistanceMile;
         VSSDataPackets[VSSPacketsNumbers[2]].m_ValidParametersCount++;
         VSSPacketsNumbers[2]++;
         break;

      case vss_pt_LeftTimeMin:
         VSSDataPackets[VSSPacketsNumbers[3]].m_LeftTimeMinIsValid = VSSData.m_LeftTimeMinIsValid;
         VSSDataPackets[VSSPacketsNumbers[3]].m_LeftTimeMin = VSSData.m_LeftTimeMin;
         VSSDataPackets[VSSPacketsNumbers[3]].m_ValidParametersCount++;
         VSSPacketsNumbers[3]++;
         break;

      case vss_pt_SpeedU:
         VSSDataPackets[VSSPacketsNumbers[4]].m_SpeedUIsValid = VSSData.m_SpeedUIsValid;
         VSSDataPackets[VSSPacketsNumbers[4]].m_SpeedU = VSSData.m_SpeedU;
         VSSDataPackets[VSSPacketsNumbers[4]].m_ValidParametersCount++;
         VSSPacketsNumbers[4]++;
         break;

      case vss_pt_TimeG:
         VSSDataPackets[VSSPacketsNumbers[5]].m_TimeGIsValid = VSSData.m_TimeGIsValid;
         VSSDataPackets[VSSPacketsNumbers[5]].m_TimeGH = VSSData.m_TimeGH;
         VSSDataPackets[VSSPacketsNumbers[5]].m_TimeGM = VSSData.m_TimeGM;
         VSSDataPackets[VSSPacketsNumbers[5]].m_TimeGS = VSSData.m_TimeGS;
         VSSDataPackets[VSSPacketsNumbers[5]].m_ValidParametersCount++;
         VSSPacketsNumbers[5]++;
         break;

      case vss_pt_Temperature:
         VSSDataPackets[VSSPacketsNumbers[6]].m_TemperatureIsValid = VSSData.m_TemperatureIsValid;
         VSSDataPackets[VSSPacketsNumbers[6]].m_Temperature = VSSData.m_Temperature;
         VSSDataPackets[VSSPacketsNumbers[6]].m_TemperatureSign = VSSData.m_TemperatureSign;
         VSSDataPackets[VSSPacketsNumbers[6]].m_ValidParametersCount++;
         VSSPacketsNumbers[6]++;
         break;

      case vss_pt_Date:
         VSSDataPackets[VSSPacketsNumbers[7]].m_DateIsValid = VSSData.m_DateIsValid;
         VSSDataPackets[VSSPacketsNumbers[7]].m_DateY = VSSData.m_DateY;
         VSSDataPackets[VSSPacketsNumbers[7]].m_DateM = VSSData.m_DateM;
         VSSDataPackets[VSSPacketsNumbers[7]].m_DateD = VSSData.m_DateD;
         VSSDataPackets[VSSPacketsNumbers[7]].m_ValidParametersCount++;
         VSSPacketsNumbers[7]++;
         break;

      case vss_pt_HeightF:
         VSSDataPackets[VSSPacketsNumbers[8]].m_HeightFIsValid = VSSData.m_HeightFIsValid;
         VSSDataPackets[VSSPacketsNumbers[8]].m_HeightF = VSSData.m_HeightF;
         VSSDataPackets[VSSPacketsNumbers[8]].m_ValidParametersCount++;
         VSSPacketsNumbers[8]++;
         break;
      }
   }

   //Количество пакетов будет равно количеству максимально поступившего параметра
   int MaxPacketsCount = 0;
   for (int n2 = 0; n2 < CVSSData::VSSParametersCount; n2++)
      if (VSSPacketsNumbers[n2] >= MaxPacketsCount)
         MaxPacketsCount = VSSPacketsNumbers[n2];
   SAFE_DELETE_ARRAY(VSSPacketsNumbers);

   for (int p = 0; p < MaxPacketsCount; p++)
   {
      CVSSData vssdata(VSSDataPackets[p]);
      _VSSDataPackets.push_back(vssdata);
   }

   SAFE_DELETE_ARRAY(VSSDataPackets);

   packets_count = MaxPacketsCount;
}


//-------------------------------------------------------------------------------
bool VSSRoutines::ReceiveVSSDataCom(QString com, std::list<CVSSData>& _VSSDataPackets)
{
#ifdef _MSC_VER
   if (osg::getNotifyLevel() > osg::NotifySeverity::FATAL)
   {
      sys::log(_T("VSSRoutines::ReceiveVSSDataCom begin\n"));
   }
   _VSSDataPackets.clear();

   VSSComReader mComReader(com,0,false);

   bool ok;
   if (ok = mComReader.readData(com))
   {
      int params_count = 0;
      int packets_count = 0;

      //принимаем данные с ВСС1
      parseVSSData(_VSSDataPackets, mComReader.mBuf1, params_count, packets_count);
      int n1 = (int)_VSSDataPackets.size();
      if (osg::getNotifyLevel() > osg::NotifySeverity::FATAL)
      {
         sys::log(_T("Received %d pakets from VSS1\n"), n1);
      }

      //принимаем данные с ВСС2
      parseVSSData(_VSSDataPackets, mComReader.mBuf2, params_count, packets_count);
      int n2 = (int)_VSSDataPackets.size() - n1;
      if (osg::getNotifyLevel() > osg::NotifySeverity::FATAL)
      {
         sys::log(_T("Received %d pakets from VSS2\n"), n2);
      }

      //сортируем принятые пакеты по количеству верных параметров
      _VSSDataPackets.sort(valid_parameters_count_sort_func());
   }

   if (osg::getNotifyLevel() > osg::NotifySeverity::FATAL)
   {
      sys::log(_T("VSSRoutines::ReceiveVSSDataCom end\n"));
   }

   return ok;
#else
   return false;
#endif
}

//-------------------------------------------------------------------------------
bool VSSRoutines::ReceiveVSSDataWeb(QString adr, std::list<CVSSData>& _VSSDataPackets)
{
   if (osg::getNotifyLevel() > osg::NotifySeverity::FATAL)
   {
      sys::log(_T("VSSRoutines::ReceiveVSSDataWeb begin\n"));
   }
   _VSSDataPackets.clear();

   // get data from server
   VSSWebReader wr(adr);
   QApplication::processEvents();
   auto start = std::chrono::system_clock::now();
   while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count() < VSS_DATA_TRANSFER_TIME_MS*5
      && !wr.isDone())
   {
      //std::this_thread::sleep_for(std::chrono::milliseconds(500));
      QThread::usleep(500);
      QApplication::processEvents();
   }

   bool ok;
   if (ok = (wr.isDone() && wr.mDatasGot==2))
   {
      int params_count = 0;
      int packets_count = 0;

      //принимаем данные с ВСС1
      parseVSSData(_VSSDataPackets, wr.mData[0], params_count, packets_count);
      int n1 = (int)_VSSDataPackets.size();
      if (osg::getNotifyLevel() > osg::NotifySeverity::FATAL)
      {
         sys::log(_T("Received %d pakets from VSS1\n"), n1);
      }

      //принимаем данные с ВСС2
      parseVSSData(_VSSDataPackets, wr.mData[1], params_count, packets_count);
      int n2 = (int)_VSSDataPackets.size() - n1;
      if (osg::getNotifyLevel() > osg::NotifySeverity::FATAL)
      {
         sys::log(_T("Received %d pakets from VSS2\n"), n2);
      }

      //сортируем принятые пакеты по количеству верных параметров
      _VSSDataPackets.sort(valid_parameters_count_sort_func());
   }

   if (osg::getNotifyLevel() > osg::NotifySeverity::FATAL)
   {
      sys::log(_T("VSSRoutines::ReceiveVSSDataWeb end\n"));
   }

   return ok;
}

