
#include "VSSProtocol.h"
#include <math.h>
#include <iostream>
#include "bit.h"


#ifndef _T
#define _T
#endif

#define BIRS_THROW throw

#define MAX(_A, _B) (_A > _B ? _A : _B)
#define MIN(_A, _B) (_A < _B ? _A : _B)
#define DELTA(_A, _B) (fabs(_A - _B))

//-------------------------------------------------------------------------------
CVSSData::CVSSData(void)
{
	m_ValidParametersCount = 0;

	//долгота
	m_LongitudeIsValid = false;
	m_LongitudeDirection = longd_East;
	m_Longitude = 0.00;

	//широта
	m_LatitudeIsValid = false;
	m_LatitudeDirection = latd_Nord;
	m_Latitude = 0.00;	

	//расстояние оставшееся до КПМ
	m_LeftDistanceMileIsValid = false;
	m_LeftDistanceMile = 0.00;

	//время оставшееся до КПМ
	m_LeftTimeMinIsValid = false;
	m_LeftTimeMin = 0.00;

	//путевая скорость
	m_SpeedUIsValid = false;
	m_SpeedU = 0.00;

	//время текущее (гринвичское)
	m_TimeGIsValid = false;
	m_TimeGH = 0;
	m_TimeGM = 0;
	m_TimeGS = 0;

	//температура наружного воздуха
	m_TemperatureIsValid = false;
	m_Temperature = 0.00;
	m_TemperatureSign = ts_Plus;

	//дата
	m_DateIsValid = false;
	m_DateD = 0;
	m_DateM = 0;
	m_DateY = 0;
		
	//высота абсюлютная
	m_HeightFIsValid = false;
	m_HeightF = 0.00;
	m_HeightFLevel = hl_Up;
}

//-------------------------------------------------------------------------------
CVSSData::CVSSData(const CVSSData& _VSSData)
{
	m_ValidParametersCount = _VSSData.m_ValidParametersCount;

	//долгота
	m_LongitudeIsValid = _VSSData.m_LongitudeIsValid;
	m_LongitudeDirection = _VSSData.m_LongitudeDirection;
	m_Longitude = _VSSData.m_Longitude;

	//широта
	m_LatitudeIsValid = _VSSData.m_LatitudeIsValid;
	m_LatitudeDirection = _VSSData.m_LatitudeDirection;
	m_Latitude = _VSSData.m_Latitude;

	//расстояние оставшееся до КПМ
	m_LeftDistanceMileIsValid = _VSSData.m_LeftDistanceMileIsValid;
	m_LeftDistanceMile = _VSSData.m_LeftDistanceMile;

	//время оставшееся до КПМ
	m_LeftTimeMinIsValid = _VSSData.m_LeftTimeMinIsValid;
	m_LeftTimeMin = _VSSData.m_LeftTimeMin;

	//путевая скорость
	m_SpeedUIsValid = _VSSData.m_SpeedUIsValid;
	m_SpeedU = _VSSData.m_SpeedU;

	//время текущее (гринвичское)
	m_TimeGIsValid = _VSSData.m_TimeGIsValid;
	m_TimeGH = _VSSData.m_TimeGH;
	m_TimeGM = _VSSData.m_TimeGM;
	m_TimeGS = _VSSData.m_TimeGS;

	//температура наружного воздуха
	m_TemperatureIsValid = _VSSData.m_TemperatureIsValid;
	m_Temperature = _VSSData.m_Temperature;
	m_TemperatureSign = _VSSData.m_TemperatureSign;

	//дата
	m_DateIsValid = _VSSData.m_DateIsValid;
	m_DateD = _VSSData.m_DateD;
	m_DateM = _VSSData.m_DateM;
	m_DateY = _VSSData.m_DateY;
	
	//высота абсюлютная
	m_HeightFIsValid = _VSSData.m_HeightFIsValid;
	m_HeightF = _VSSData.m_HeightF;
	m_HeightFLevel = _VSSData.m_HeightFLevel;
}

//----------------------------------------------------------------------------------------------
//проверяет параметры
void CVSSData::ValidateAll(void)
{
	if (m_TemperatureIsValid)
	{
		if (m_Temperature > 70.00)
			m_Temperature = false;
	}

	//нереальная скорость (не больше 1000 км)
	if (m_SpeedUIsValid)
	{
		if (m_SpeedU > 600.00)
			m_SpeedUIsValid = false;
	}

	//если есть скорость то высота, оставшееся до КПМ время и расстояние должны быть > 0, иначе 0
	if (m_SpeedUIsValid && m_SpeedU > 0.00)
	{
		if (m_LeftDistanceMile == 0.00)
			m_LeftDistanceMileIsValid = false;
		if (m_LeftTimeMin == 0.00)
			m_LeftTimeMinIsValid = false;
		if (m_HeightF == 0.00)
			m_HeightFIsValid = false;
	}
}


//-------------------------------------------------------------------------------
CVSSData::~CVSSData(void)
{
}

//-------------------------------------------------------------------------------
CVSSProtocol::CVSSProtocol(void)
{
}

//-------------------------------------------------------------------------------
//проверка CRC: бит четности дополняет до НЕчетного числа единиц
bool CVSSProtocol::CheckCRC(DWORD Parameter)
{
	BYTE n1 = bit::CalcBits1Count(Parameter);
	return (n1 % 2 == 1);
}

//-------------------------------------------------------------------------------
//возвращение типа значения
TVSSParameterType CVSSProtocol::DecodeParameterType(DWORD Parameter)
{
	TVSSParameterType pt = vss_pt_Unknown;

	DWORD Type = Parameter & TYPE;
	switch(Type)
	{
	case vss_pt_Longitude:		
		pt = vss_pt_Longitude;
		break;
	case vss_pt_Latitude:		
		pt = vss_pt_Latitude;
		break;
	case vss_pt_LeftDistanceMile:
   case 0x97:
		pt = vss_pt_LeftDistanceMile;
		break;
	case vss_pt_LeftTimeMin:
   case 0x57:
		pt = vss_pt_LeftTimeMin;
		break;
	case vss_pt_SpeedU:
		pt = vss_pt_SpeedU;
		break;
	case vss_pt_TimeG:
		pt = vss_pt_TimeG;
		break;
	case vss_pt_Temperature:
		pt = vss_pt_Temperature;
		break;
	case vss_pt_Date:
		pt = vss_pt_Date;
		break;
	case vss_pt_HeightF:
		pt = vss_pt_HeightF;
		break;
   default:
      pt = vss_pt_Unknown;
      break;
	}

	return pt;
}

//-------------------------------------------------------------------------------
//долгота (распаковка)
bool CVSSProtocol::DecodeLongitude(DWORD Parameter, double &_Longitude, TLongitudeDirection &_LongitudeDirection)
{
	if (!CheckCRC(Parameter))
		return false;

	DWORD Status = Parameter & STATUS;
	if (!((Status & vss_ps_OK) == vss_ps_OK))
		return false;

	TLongitudeDirection LongitudeDirection = (TLongitudeDirection)bit::GetSubDWORD(Parameter, 29, 29);
	DWORD dwLongitude = bit::GetSubDWORD(Parameter, 9, 28);
	if (LongitudeDirection == longd_West)
		dwLongitude = 1048576 - dwLongitude;
	double k = 0.000171661376953125; //(90/2^19)
	double Longitude = k*((double)(dwLongitude));
	if (!(Longitude >= 0.00 && Longitude <= 180.00))
		return false;
	
	_LongitudeDirection = LongitudeDirection;
	_Longitude = Longitude;

	return true;
}

//-------------------------------------------------------------------------------
//долгота (упаковка)
DWORD CVSSProtocol::EncodeLongitude(double _Longitude, TLongitudeDirection _LongitudeDirection)
{
	if (_Longitude < 0.00 || _Longitude > 180.00)
		BIRS_THROW((_T("err_WRONG_PARAMETER_LONGITUDE")));
	DWORD Parameter = 0;
	
	DWORD LongitudeDirection  = (_LongitudeDirection == longd_East ? 0 : 1) << 28;
	double k = 0.000171661376953125; //(90/2^19)
	DWORD Longitude = (DWORD)(_Longitude / k);
	if (_LongitudeDirection == longd_West)
		Longitude = 1048576 - Longitude;
	Longitude = Longitude << 8;
	Parameter = vss_ps_OK | Longitude | LongitudeDirection | vss_pt_Longitude;
	BYTE n1 = bit::CalcBits1Count(Parameter);
	if (n1 % 2 == 0)
		Parameter = Parameter | 0x80000000;

	TLongitudeDirection LongitudeDirection1;
	double Longitude1;
	if (!DecodeLongitude(Parameter, Longitude1, LongitudeDirection1) || DELTA(Longitude1, _Longitude) > EPSILON || (LongitudeDirection1 != _LongitudeDirection))
		BIRS_THROW((_T("err_WRONG_LONGITUDE_PACK")));

	return Parameter;
}


//-------------------------------------------------------------------------------
//широта (распаковка)
bool CVSSProtocol::DecodeLatitude(DWORD Parameter, double &_Latitude, TLatitudeDirection &_LatitudeDirection)
{
	if (!CheckCRC(Parameter))
		return false;

	DWORD Status = Parameter & STATUS;
	if (!((Status & vss_ps_OK) == vss_ps_OK))
		return false;

	TLatitudeDirection LatitudeDirection = (TLatitudeDirection)bit::GetSubDWORD(Parameter, 29, 29);
	DWORD dwLatitude = bit::GetSubDWORD(Parameter, 9, 28);
	if (LatitudeDirection == latd_South)
		dwLatitude = 1048576 - dwLatitude;
	double k = 0.000171661376953125; //(90/2^19)
	double Latitude = k*((double)(dwLatitude));
	if (!(Latitude >= 0.00 && Latitude <= 90.00))
		return false;
	
	_Latitude = Latitude;
	_LatitudeDirection = LatitudeDirection;
	return true;
}

//-------------------------------------------------------------------------------
//широта (упаковка)
DWORD CVSSProtocol::EncodeLatitude(double _Latitude, TLatitudeDirection _LatitudeDirection)
{
	if (_Latitude < 0.00 || _Latitude > 90.00)
		BIRS_THROW((_T("err_WRONG_PARAMETER_LATITUDE")));
	DWORD Parameter = 0;
	
	DWORD LatitudeDirection  = (_LatitudeDirection == longd_East ? 0 : 1) << 28;
	double k = 0.000171661376953125; //(90/2^19)
	DWORD Latitude = (DWORD)(_Latitude / k);
	if (_LatitudeDirection == latd_South)
		Latitude = 1048576 - Latitude;
	Latitude = Latitude << 8;
	Parameter = vss_ps_OK | Latitude | LatitudeDirection | vss_pt_Latitude;
	BYTE n1 = bit::CalcBits1Count(Parameter);
	if (n1 % 2 == 0)
		Parameter = Parameter | 0x80000000;

	TLatitudeDirection LatitudeDirection1;
	double Latitude1;
	if (!DecodeLatitude(Parameter, Latitude1, LatitudeDirection1) || DELTA(Latitude1, _Latitude) > EPSILON || (LatitudeDirection1 != _LatitudeDirection))
		BIRS_THROW((_T("err_WRONG_LATITUDE_PACK")));

	return Parameter;
}


//-------------------------------------------------------------------------------
//расстояние, оставшееся до КПМ (распаковка)
bool CVSSProtocol::DecodeLeftDistanceMile(DWORD Parameter, double &_LeftDistanceMile)
{
	if (!CheckCRC(Parameter))
		return false;

	DWORD Status = Parameter & STATUS;
	if (!((Status & vss_ps_OK) == vss_ps_OK))
		return false;

	double k = 0.125;
	double LeftDistanceMile = k*((double)(bit::GetSubDWORD(Parameter, 11, 28)));
	if (!(LeftDistanceMile >= 0.00 && LeftDistanceMile <= 16384.00))
		return false;

	_LeftDistanceMile = LeftDistanceMile;
	return true;
}

//-------------------------------------------------------------------------------
//расстояние, оставшееся до КПМ (упаковка)
DWORD CVSSProtocol::EncodeLeftDistanceMile(double _LeftDistanceMile, TVSSID _VSSID)
{
	if (_LeftDistanceMile < 0.00 || _LeftDistanceMile > 16384.00)
		BIRS_THROW((_T("err_WRONG_PARAMETER_LEFTDISTANCEMILE")));
	
	DWORD Parameter = 0;

	DWORD SignBit = 0 << 28;
	double k = 0.125; //(16384/2^16)
	DWORD LeftDistanceMile = ((DWORD)((_LeftDistanceMile / k))) << 10;
	DWORD VSSID = _VSSID == ID_VSS1 ? ((DWORD)(1)) << 9 : ((DWORD)(1)) << 8;
	Parameter = vss_ps_OK | SignBit | LeftDistanceMile | VSSID | vss_pt_LeftDistanceMile;
	BYTE n1 = bit::CalcBits1Count(Parameter);
	if (n1 % 2 == 0)
		Parameter = Parameter | 0x80000000;

	double LeftDistanceMile1;
	if (!DecodeLeftDistanceMile(Parameter, LeftDistanceMile1) || DELTA(LeftDistanceMile1, _LeftDistanceMile) > EPSILON)
		BIRS_THROW((_T("err_WRONG_LEFTDISTANCEMILE_PACK")));

	return Parameter;
}

//-------------------------------------------------------------------------------
//время, оставшееся до КПМ (распаковка)
bool CVSSProtocol::DecodeLeftTimeMin(DWORD Parameter, double &_LeftTimeMin)
{
	if (!CheckCRC(Parameter))
		return false;

	DWORD Status = Parameter & STATUS;
	if (!((Status & vss_ps_OK) == vss_ps_OK))
		return false;

	double k = 1.00;
	double LeftTimeMin = k*((double)(bit::GetSubDWORD(Parameter, 17, 28)));
	if (!(LeftTimeMin >= 0.00 && LeftTimeMin <= 2048))
		return false;

	_LeftTimeMin = LeftTimeMin;
	return true;
}

//-------------------------------------------------------------------------------
//время, оставшееся до КПМ (упаковка)
DWORD CVSSProtocol::EncodeLeftTimeMin(double _LeftTimeMin, TVSSID _VSSID)
{
	if (_LeftTimeMin < 0.00 || _LeftTimeMin > 2048)
		 BIRS_THROW((_T("err_WRONG_PARAMETER_LEFTTIMEMIN")));
	DWORD Parameter = 0;

	DWORD SignBit = 0 << 28;
	double k = 1.00; //(2048/2^11)
	DWORD LeftTimeMin = ((DWORD)((_LeftTimeMin / k))) << 16;
	DWORD VSSID = _VSSID == ID_VSS1 ? ((DWORD)(1)) << 9 : ((DWORD)(1)) << 8;
	Parameter = vss_ps_OK | SignBit | LeftTimeMin | VSSID | vss_pt_LeftTimeMin;
	BYTE n1 = bit::CalcBits1Count(Parameter);
	if (n1 % 2 == 0)
		Parameter = Parameter | 0x80000000;

	double LeftTimeMin1;
	if (!DecodeLeftTimeMin(Parameter, LeftTimeMin1) || DELTA(LeftTimeMin1, _LeftTimeMin) > EPSILON)
		 BIRS_THROW((_T("err_WRONG_LEFTTIMEMIN_PACK")));

	return Parameter;
}

//-------------------------------------------------------------------------------
//путевая скорость (распаковка)
bool CVSSProtocol::DecodeSpeedU(DWORD Parameter, double &_SpeedU)
{
	if (!CheckCRC(Parameter))
		return false;

	DWORD Status = Parameter & STATUS;
	if (!((Status & vss_ps_OK) == vss_ps_OK))
		return false;

	double k = 0.125;
	double SpeedU = k*((double)(bit::GetSubDWORD(Parameter, 14, 28)));
	if (!(SpeedU >= 0.00 && SpeedU <= 4096.00))
		return false;

	_SpeedU = SpeedU;
	return true;
}

//-------------------------------------------------------------------------------
//путевая скорость (упаковка)
DWORD CVSSProtocol::EncodeSpeedU(double _SpeedU, TVSSID _VSSID)
{
	if (_SpeedU < 0.00 || _SpeedU > 4096.00)
		 BIRS_THROW((_T("err_WRONG_PARAMETER_SPEEDU")));
	DWORD Parameter = 0;

	DWORD SignBit = 0 << 28;
	double k = 0.125; //(2048/2^14)
	DWORD SpeedU = ((DWORD)((_SpeedU / k))) << 13;
	DWORD VSSID = _VSSID == ID_VSS1 ? ((DWORD)(1)) << 9 : ((DWORD)(1)) << 8;
	Parameter = vss_ps_OK | SignBit | SpeedU | VSSID | vss_pt_SpeedU;
	BYTE n1 = bit ::CalcBits1Count(Parameter);
	if (n1 % 2 == 0)
		Parameter = Parameter | 0x80000000;

	double Speed1U;
	if (!DecodeSpeedU(Parameter, Speed1U) || DELTA(Speed1U, _SpeedU) > EPSILON)
		 BIRS_THROW((_T("err_WRONG_SPEEDU_PACK")));

	return Parameter;
}

//-------------------------------------------------------------------------------
//время текущее (гринвичское) (распаковка)
bool CVSSProtocol::DecodeTimeG(DWORD Parameter, BYTE &_TimeGH, BYTE &_TimeGM, BYTE &_TimeGS)
{
	if (!CheckCRC(Parameter))
		return false;

	DWORD Status = Parameter & STATUS;
	if (!((Status & vss_ps_OK) == vss_ps_OK))
		return false;

	BYTE TimeGH = ((BYTE)(bit::GetSubDWORD(Parameter, 24, 28)));
	if (!(TimeGH >= 0 && TimeGH <= 23))
		return false;
	BYTE TimeGM = ((BYTE)(bit::GetSubDWORD(Parameter, 18, 23)));
	if (!(TimeGM >= 0 && TimeGM <= 59))
		return false;
	BYTE TimeGS = ((BYTE)(bit::GetSubDWORD(Parameter, 12, 17)));
	if (!(TimeGS >= 0 && TimeGS <= 59))
		return false;

	_TimeGH = TimeGH+3;
	_TimeGM = TimeGM;
	_TimeGS = TimeGS;
	return true;
}

//-------------------------------------------------------------------------------
//время текущее (гринвичское) (упаковка)
DWORD CVSSProtocol::EncodeTimeG(BYTE _TimeGH, BYTE _TimeGM, BYTE _TimeGS, TVSSID _VSSID)
{
	if ((_TimeGH < 0 || _TimeGH > 23) || (_TimeGM < 0 || _TimeGM > 59) || (_TimeGS < 0 || _TimeGS > 59))
		 BIRS_THROW((_T("err_WRONG_PARAMETER_TIMEG")));
	DWORD Parameter = 0;

	double kH = 1.00; // (16/2^4)
	double kM = 1.00; // (32/2^5)
	double kS = 1.00; // (32/2^5)

	DWORD TimeGH = ((DWORD)(_TimeGH / kH)) << 23;
	DWORD TimeGM = ((DWORD)(_TimeGM / kM)) << 17;
	DWORD TimeGS = ((DWORD)(_TimeGS / kS)) << 11;

	DWORD VSSID = _VSSID == ID_VSS1 ? ((DWORD)(1)) << 9 : ((DWORD)(1)) << 8;

	Parameter = vss_ps_OK | TimeGH | TimeGM | TimeGS | VSSID | vss_pt_TimeG;
	BYTE n1 = bit::CalcBits1Count(Parameter);
	if (n1 % 2 == 0)
		Parameter = Parameter | 0x80000000;

	BYTE TimeGH1, TimeGM1, TimeGS1;
	if (!DecodeTimeG(Parameter, TimeGH1, TimeGM1, TimeGS1) ||
		TimeGH1 - _TimeGH != 0 ||
		TimeGM1 - _TimeGM != 0 ||
		TimeGS1 - _TimeGS != 0)
		 BIRS_THROW((_T("err_WRONG_TIMEG_PACK")));

	return Parameter;
}

//-------------------------------------------------------------------------------
//температура наружного воздуха (распаковка)
bool CVSSProtocol::DecodeTemperature(DWORD Parameter, double& _Temperature, TTemperatureSign& _TemperatureSign)
{
	if (!CheckCRC(Parameter))
		return false;

	DWORD Status = Parameter & STATUS;
	if (!((Status & vss_ps_OK) == vss_ps_OK))
		return false;

	DWORD dwTemperature = bit::GetSubDWORD(Parameter, 18, 28);
	TTemperatureSign TemperatureSign = (TTemperatureSign)bit::GetSubDWORD(Parameter, 29, 29);

	if (TemperatureSign == ts_Minus && dwTemperature > 0)
		dwTemperature = 2048 - dwTemperature;
	double k = 0.25;
	double Temperature = k*dwTemperature;
	if (!(Temperature >= 0.00 && Temperature <= 512.00))
		return false;

	_Temperature = Temperature;
	_TemperatureSign = TemperatureSign;

	return true;
}

//-------------------------------------------------------------------------------
//температура наружного воздуха (упаковка)
DWORD CVSSProtocol::EncodeTemperature(double _Temperature, TTemperatureSign _TemperatureSign, TVSSID _VSSID)
{
	if (_Temperature < 0.00 || _Temperature > 512.00)
		 BIRS_THROW((_T("err_WRONG_PARAMETER_TEMPERATURE")));
	DWORD Parameter = 0;

	double k = 0.25; //(256/2^10)
	DWORD Temperature = (DWORD)(_Temperature / k);
	if (_TemperatureSign == ts_Minus && _Temperature > 0)
		Temperature = 2048 - Temperature;
	Temperature = Temperature << 18;
	DWORD VSSID = _VSSID == ID_VSS1 ? ((DWORD)(1)) << 9 : ((DWORD)(1)) << 8;
	DWORD TemperatureSign = (_TemperatureSign == ts_Minus ? 1 : 0) << 28;
	Parameter = vss_ps_OK | TemperatureSign | Temperature | VSSID | vss_pt_Temperature;
	BYTE n1 = bit::CalcBits1Count(Parameter);
	if (n1 % 2 == 0)
		Parameter = Parameter | 0x80000000;

	TTemperatureSign TemperatureSign1;
	double Temperature1;
	if (!DecodeTemperature(Parameter, Temperature1, TemperatureSign1) || DELTA(Temperature1, _Temperature) > EPSILON || (TemperatureSign1 != _TemperatureSign))
		 BIRS_THROW((_T("err_WRONG_TEMPERATURE_PACK")));

	return Parameter;
}

//-------------------------------------------------------------------------------
//дата (распаковка)
bool CVSSProtocol::DecodeDate(DWORD Parameter, BYTE& _DateD, BYTE& _DateM, BYTE& _DateY)
{
	if (!CheckCRC(Parameter))
		return false;

	DWORD Status = Parameter & STATUS;
	if (!((Status | vss_ps_OKDate) == vss_ps_OKDate))
		return false;

	BYTE DateY = 10*(BYTE)bit::GetSubDWORD(Parameter, 15, 18) + (BYTE)bit::GetSubDWORD(Parameter, 11, 14);
	if (!(DateY >= 1 && DateY <= 99))
		return false;
	BYTE DateM = 10*(BYTE)bit::GetSubDWORD(Parameter, 23, 23) + (BYTE)bit::GetSubDWORD(Parameter, 19, 22);
	if (!(DateM >= 1 && DateM <= 12))
		return false;
	BYTE DateD = 10*(BYTE)bit::GetSubDWORD(Parameter, 28, 29) + (BYTE)bit::GetSubDWORD(Parameter, 24, 27);
	if (!(DateD >= 1 && DateD <= 31))
		return false;

	_DateY = DateY;
	_DateM = DateM;
	_DateD = DateD;
	return true;
}

//-------------------------------------------------------------------------------
//дата (упаковка)
DWORD CVSSProtocol::EncodeDate(BYTE _DateD, BYTE _DateM, BYTE _DateY, TVSSID _VSSID)
{
	if (_DateD < 1 || _DateD > 31 || _DateM < 1 || _DateM > 12)
		BIRS_THROW((_T("err_WRONG_PARAMETER_DATE")));
	DWORD Parameter = 0;
	
	DWORD Day10 = ((DWORD)(_DateD / 10)) << 27;
	DWORD Day1 = ((DWORD)(_DateD % 10)) << 23;
	DWORD Month10 = ((DWORD)(_DateM / 10)) << 22;
	DWORD Month1 = ((DWORD)(_DateM % 10)) << 18;
	DWORD Year10 = ((DWORD)(_DateY / 10)) << 14;
	DWORD Year1 = ((DWORD)(_DateY % 10)) << 10;
	DWORD VSSID = _VSSID == ID_VSS1 ? ((DWORD)(1)) << 9 : ((DWORD)(1)) << 8;

	Parameter = Day10 | Day1 | Month10 | Month1 | Year10 | Year1 | VSSID | vss_pt_Date;
	BYTE n1 = bit::CalcBits1Count(Parameter);
	if (n1 % 2 == 0)
		Parameter = Parameter | 0x80000000;

	BYTE DateD, DateM, DateY;
	if (!DecodeDate(Parameter, DateD, DateM, DateY) || DateD != _DateD || DateM != _DateM || DateY != DateY)
		BIRS_THROW((_T("err_WRONG_DATE_PACK")));

	return Parameter;
}

//-------------------------------------------------------------------------------
//высота абсюлютная (распаковка)
bool CVSSProtocol::DecodeHeightF(DWORD Parameter, double &_HeightF, THeightFLevel& _HeightFLevel)
{
	if (!CheckCRC(Parameter))
		return false;

	DWORD Status = Parameter & STATUS;
	if (!((Status & vss_ps_OK) == vss_ps_OK))
		return false;

	double k = 1.00;
	double HeightF = k*((double)(bit::GetSubDWORD(Parameter, 12, 28)));
	if (!(HeightF >= 0.00 && HeightF <= 131072.00))
		return false;

	_HeightF = HeightF;
	_HeightFLevel = (THeightFLevel)bit::GetSubDWORD(Parameter, 29, 29);
	return true;
}

//-------------------------------------------------------------------------------
//высота абсюлютная (упаковка)
DWORD CVSSProtocol::EncodeHeightF(double _Height, THeightFLevel _HeightFLevel, TVSSID _VSSID)
{
	if (_Height < 0.00 || _Height > 131072.00)
		 BIRS_THROW((_T("err_WRONG_PARAMETER_HEIGHTF")));
	DWORD Parameter = 0;

	double k = 1.0; //(65536/2^16)
	DWORD HeightFLevel = (_HeightFLevel == hl_Down ? 1 : 0) << 28;
	DWORD VSSID = _VSSID == ID_VSS1 ? ((DWORD)(1)) << 9 : ((DWORD)(1)) << 8;
	DWORD Height = ((DWORD)((_Height / k))) << 11;
	Parameter = vss_ps_OK | HeightFLevel | Height | VSSID | vss_pt_HeightF;
	BYTE n1 = bit::CalcBits1Count(Parameter);
	if (n1 % 2 == 0)
		Parameter = Parameter | 0x80000000;

	THeightFLevel HeightFLevel1;
	double Height1;
	if (!DecodeHeightF(Parameter, Height1, HeightFLevel1) || DELTA(Height1, _Height) > EPSILON || (HeightFLevel1 != _HeightFLevel))
		 BIRS_THROW((_T("err_WRONG_HEIGHTF_PACK")));

	return Parameter;
}

//-------------------------------------------------------------------------------
//параметр (распаковка)
bool CVSSProtocol::DecodeParameter(DWORD Parameter, CVSSData* _pVSSData)
{
	//std::cout << (_T("CVSSProtocol::DecodeParameter(%X)"), Parameter);

	TVSSParameterType pt = DecodeParameterType(Parameter);
	switch(pt)
	{
	case vss_pt_Longitude:
		TLongitudeDirection LongitudeDirection;
		double Longitude;
		if (!(_pVSSData->m_LongitudeIsValid = DecodeLongitude(Parameter, Longitude, LongitudeDirection)))
			return false;
		_pVSSData->m_LongitudeDirection = LongitudeDirection;
		_pVSSData->m_Longitude = Longitude;
		_pVSSData->m_ValidParametersCount++;
		return true;

	case vss_pt_Latitude:
		TLatitudeDirection LatitudeDirection;
		double Latitude;					
		if (!(_pVSSData->m_LatitudeIsValid = DecodeLatitude(Parameter, Latitude, LatitudeDirection)))
			return false;
		_pVSSData->m_LatitudeDirection = LatitudeDirection;
		_pVSSData->m_Latitude = Latitude;
		_pVSSData->m_ValidParametersCount++;
		return true;

	case vss_pt_LeftDistanceMile:
		double LeftDistanceMile;
		if (!(_pVSSData->m_LeftDistanceMileIsValid = DecodeLeftDistanceMile(Parameter, LeftDistanceMile)))
			return false;
		_pVSSData->m_LeftDistanceMile = LeftDistanceMile;
		_pVSSData->m_ValidParametersCount++;
		return true;

	case vss_pt_LeftTimeMin:
		double LeftTimeMin;
		if (!(_pVSSData->m_LeftTimeMinIsValid = DecodeLeftTimeMin(Parameter, LeftTimeMin)))
			return false;
		_pVSSData->m_LeftTimeMin = LeftTimeMin;
		_pVSSData->m_ValidParametersCount++;
		return true;

	case vss_pt_SpeedU:
		double SpeedU;
		if (!(_pVSSData->m_SpeedUIsValid = DecodeSpeedU(Parameter, SpeedU)))
			return false;
		_pVSSData->m_SpeedU = SpeedU;
		_pVSSData->m_ValidParametersCount++;
		return true;

	case vss_pt_TimeG:
		BYTE TimeGH;
		BYTE TimeGM;
		BYTE TimeGS;
		if (!(_pVSSData->m_TimeGIsValid = DecodeTimeG(Parameter, TimeGH, TimeGM, TimeGS)))
			return false;
		_pVSSData->m_TimeGH = TimeGH;
		_pVSSData->m_TimeGM = TimeGM;
		_pVSSData->m_TimeGS = TimeGS;
		_pVSSData->m_ValidParametersCount++;
		return true;

	case vss_pt_Temperature:
		TTemperatureSign TemperatureSign;
		double Temperature;		
		if (!(_pVSSData->m_TemperatureIsValid = DecodeTemperature(Parameter, Temperature, TemperatureSign)))
			return false;
		_pVSSData->m_TemperatureSign = TemperatureSign;
		_pVSSData->m_Temperature = Temperature;		
		_pVSSData->m_ValidParametersCount++;
		return true;

	case vss_pt_Date:
		BYTE DateD;
		BYTE DateM;
		BYTE DateY;
		if (!(_pVSSData->m_DateIsValid = DecodeDate(Parameter, DateD, DateM, DateY)))
			return false;
		_pVSSData->m_DateD = DateD;
		_pVSSData->m_DateM = DateM;
		_pVSSData->m_DateY = DateY;
		_pVSSData->m_ValidParametersCount++;
		return true;

	case vss_pt_HeightF:
		THeightFLevel HeightFLevel;
		double HeightF;
		if (!(_pVSSData->m_HeightFIsValid = DecodeHeightF(Parameter, HeightF, HeightFLevel)))
			return false;
		_pVSSData->m_HeightFLevel = HeightFLevel;
		_pVSSData->m_HeightF = HeightF;
		_pVSSData->m_ValidParametersCount++;
		return true;

	default:
		return false;
	}
}

//-------------------------------------------------------------------------------
CVSSProtocol::~CVSSProtocol(void)
{
}
