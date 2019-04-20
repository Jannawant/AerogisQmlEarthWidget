#pragma once

#ifndef DWORD
typedef unsigned long DWORD;
#endif

namespace bit
{
	//-------------------------------------------------------------------------------------------
	//возвращает количество бит 1 в слове
	inline BYTE CalcBits1Count(DWORD _dwValue)
	{
		BYTE n1 = 0;
		while(_dwValue != 0)
		{
			n1 = n1 + (BYTE)(_dwValue & 0x00000001);
			_dwValue = _dwValue >> 1;
		}

		return n1;
	}

	//----------------------------------------------------------------------------------------------
	//возвращает слово, заключенное между битами в слове [1..32]
	inline DWORD GetSubDWORD(DWORD _dwValue, BYTE _from, BYTE _to)
	{
		if(!(_from >= 1 && _from <= 32 && _to >= _from && _to <= 32))
			return -1;
		DWORD dw = _dwValue;
		BYTE nleft = 32 - _to;
		dw = dw << nleft;
		BYTE nright = nleft + _from - 1;
		dw = dw >> nright;
		return dw;
	}

	//----------------------------------------------------------------------------------------------
	//из 4-х байтов формирует слово
	inline DWORD CreateDWORD(BYTE _b1, BYTE _b2, BYTE _b3, BYTE _b4)
	{
		DWORD dw4 = ((DWORD)_b4) << 24;
		DWORD dw3 = ((DWORD)_b3) << 16;
		DWORD dw2 = ((DWORD)_b2) << 8;
		DWORD dw1 = _b1;
		return (dw4 | dw3 | dw2 | dw1);		
	}

	//-------------------------------------------------------------------------------------------
	//возвращает true если бит n установлен в 1 (n [1..32])
	inline bool TestBit(DWORD _dwValue, int _n)
	{
		return GetSubDWORD(_dwValue, _n, _n) == 1;
	}
}