#pragma once

//#include <windows.h>
//#include <tchar.h>

typedef unsigned char BYTE;
typedef unsigned long DWORD;

//-------------------------------------------------------------------------------
#define TYPE 0x000000FF
#define STATUS 0x60000000
#define EPSILON 0.001

//-------------------------------------------------------------------------------
enum TVSSID
{
	ID_VSS1 = 0x01,
	ID_VSS2 = 0x02,
};

//-------------------------------------------------------------------------------
enum TVSSParameterState
{
	vss_ps_Fail = 0x00000000,
	vss_ps_NoData = 0x40000000,
	vss_ps_FuncTest = 0x20000000,
	vss_ps_OK = 0x60000000,
	vss_ps_OKDate = 0x9FFFFFFF
};

//-------------------------------------------------------------------------------
enum TVSSParameterType
{
	vss_pt_Longitude = 0x00000093,
	vss_pt_Latitude = 0x00000013,
	//vss_pt_LeftDistanceMile = 0x000097,
	//vss_pt_LeftTimeMin = 0x000057,
	
	vss_pt_LeftDistanceMile = 0x000000A0,
	vss_pt_LeftTimeMin = 0x00000060,
	
	vss_pt_SpeedU = 0x00000053,
	vss_pt_TimeG = 0x00000016,
	vss_pt_Temperature = 0x000000D1,
	vss_pt_Date = 0x0000000D,
	vss_pt_HeightF = 0x000000C1,
	vss_pt_Unknown = 0x00000000
};

//-------------------------------------------------------------------------------
enum TLatitudeDirection
{
	latd_Nord = 0,
	latd_South = 1
};

//-------------------------------------------------------------------------------
enum TLongitudeDirection
{
	longd_East = 0,
	longd_West = 1
};

//-------------------------------------------------------------------------------
enum TTemperatureSign
{
	ts_Plus = 0,
	ts_Minus = 1
};

//-------------------------------------------------------------------------------
enum THeightFLevel
{
	hl_Up = 0,
	hl_Down = 1
};


//-------------------------------------------------------------------------------
class CVSSData
{
friend class CVSSProtocol;
friend class VSSRoutines;

protected:
	int m_ValidParametersCount;

	//долгота
	bool m_LongitudeIsValid;
	TLongitudeDirection m_LongitudeDirection;
	double m_Longitude;

	//широта
	bool m_LatitudeIsValid;
	TLatitudeDirection m_LatitudeDirection;
	double m_Latitude;

	//расстояние оставшееся до КПМ (миля.)
   // in sea miles, 1 sea mile = 1852 m
	bool m_LeftDistanceMileIsValid;
	double m_LeftDistanceMile;

	//время оставшееся до КПМ (мин.)
	bool m_LeftTimeMinIsValid;
	double m_LeftTimeMin;

	//путевая скорость (м/с)
   // in Uzls, 1 U = 1.852 km/h
	bool m_SpeedUIsValid;
	double m_SpeedU;

	//время текущее (гринвичское)
	bool m_TimeGIsValid;
	BYTE m_TimeGH;
	BYTE m_TimeGM;
	BYTE m_TimeGS;

	//температура наружного воздуха
	bool m_TemperatureIsValid;
	double m_Temperature;
	TTemperatureSign m_TemperatureSign;

	//дата
	bool m_DateIsValid;
	BYTE m_DateD;
	BYTE m_DateM;
	BYTE m_DateY;

	//высота абсюлютная
   // in feet, 1 ft = 0.3048 m
	bool m_HeightFIsValid;
	double m_HeightF;
	THeightFLevel m_HeightFLevel;

public:
	static const int VSSParametersCount = 11; //количество данных с ВСС, обрабатываемых протоколом

   virtual int ValidParametersCount(void)
   {
      return m_ValidParametersCount;
   }
   virtual bool LongitudeIsValid(void)
   {
      return m_LongitudeIsValid;
   }
   virtual double Longitude(void)
	{
		return m_Longitude;
	}
	virtual TLongitudeDirection LongitudeDirection(void)
	{
		return m_LongitudeDirection;
	}

	virtual bool LatitudeIsValid(void)
	{
		return m_LatitudeIsValid;
	}
	virtual double Latitude(void)
	{
		return m_Latitude;
	}
	virtual TLatitudeDirection LatitudeDirection(void)
	{
		return m_LatitudeDirection;
	}

	virtual bool LeftDistanceMileIsValid(void)
	{
		return m_LeftDistanceMileIsValid;
	}
	virtual double LeftDistanceMile(void)
	{
		return m_LeftDistanceMile;
	}

	virtual bool LeftTimeMinIsValid(void)
	{
		return m_LeftTimeMinIsValid;
	}
	virtual double LeftTimeMin(void)
	{
		return m_LeftTimeMin;
	}

	virtual bool SpeedUIsValid(void)
	{
		return m_SpeedUIsValid;
	}
	virtual double SpeedU(void)
	{
		return m_SpeedU;
	}

	virtual bool TimeGIsValid(void)
	{
		return m_TimeGIsValid;
	}
	virtual BYTE TimeGH(void)
	{
		return m_TimeGH;
	}
	virtual BYTE TimeGM(void)
	{
		return m_TimeGM;
	}
	virtual BYTE TimeGS(void)
	{
		return m_TimeGS;
	}

	virtual bool DateIsValid(void)
	{
		return m_DateIsValid;
	}
	virtual BYTE DateD(void)
	{
		return m_DateD;
	}
	virtual BYTE DateM(void)
	{
		return m_DateM;
	}
	virtual BYTE DateY(void)
	{
		return m_DateY;
	}

	virtual bool TemperatureIsValid(void)
	{
		return m_TemperatureIsValid;
	}
	virtual double Temperature(void)
	{
		return m_Temperature;
	}
	virtual TTemperatureSign TemperatureSign(void)
	{
		return m_TemperatureSign;
	}

	virtual bool HeightFIsValid(void)
	{
		return m_HeightFIsValid;
	}
	virtual double HeightF(void)
	{
		return m_HeightF;
	}
	virtual THeightFLevel HeightFLevel(void)
	{
		return m_HeightFLevel;
	}

	CVSSData(void);
	CVSSData(const CVSSData& _VSSData);
	virtual void ValidateAll(void);
	virtual ~CVSSData(void);
};

//-------------------------------------------------------------------------------
class CVSSProtocol
{
protected:
	CVSSProtocol(void);	
	~CVSSProtocol(void);

public:
	static bool CheckCRC(DWORD Parameter);
	static bool DecodeLongitude(DWORD Parameter, double &_Longitude, TLongitudeDirection &_LongitudeDirection);
	static DWORD EncodeLongitude(double _Longitude, TLongitudeDirection _LongitudeDirection);
	static bool DecodeLatitude(DWORD Parameter, double &_Latitude, TLatitudeDirection &_LatitudeDirection);	
	static DWORD EncodeLatitude(double _Latitude, TLatitudeDirection _LatitudeDirection);
	static bool DecodeLeftDistanceMile(DWORD Parameter, double &_LeftDistanceMile);
	static DWORD EncodeLeftDistanceMile(double _LeftDistanceMile, TVSSID _VSSID);
	static bool DecodeLeftTimeMin(DWORD Parameter, double &_LeftTimeMin);
	static DWORD EncodeLeftTimeMin(double _LeftTimeMin, TVSSID _VSSID);
	static bool DecodeSpeedU(DWORD Parameter, double &_SpeedU);
	static DWORD EncodeSpeedU(double _SpeedU, TVSSID _VSSID);
	static bool DecodeTimeG(DWORD Parameter, BYTE &_TimeGH, BYTE &_TimeGM, BYTE &_TimeGS);
	static DWORD EncodeTimeG(BYTE _TimeGH, BYTE _TimeGM, BYTE _TimeGS, TVSSID _VSSID);
	static bool DecodeTemperature(DWORD Parameter, double &_Temperature, TTemperatureSign& _TemperatureSign);
	static DWORD EncodeTemperature(double _Temperature, TTemperatureSign _TemperatureSign, TVSSID _VSSID);
	static bool DecodeDate(DWORD Parameter, BYTE &_DateD, BYTE &_DateM, BYTE &_DateY);
	static DWORD EncodeDate(BYTE _DateD, BYTE _DateM, BYTE _DateY, TVSSID _VSSID);
	static bool DecodeHeightF(DWORD Parameter, double &_HeightF, THeightFLevel& _HeightFLevel);
	static DWORD EncodeHeightF(double _HeightF, THeightFLevel _HeightFLevel, TVSSID _VSSID);
	static bool DecodeParameter(DWORD Parameter, CVSSData* _pVSSData);
	static TVSSParameterType DecodeParameterType(DWORD Parameter);
};