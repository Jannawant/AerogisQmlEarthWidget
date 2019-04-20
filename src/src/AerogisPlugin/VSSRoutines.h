#pragma once

#include <list>
#include "VSSProtocol.h"
#include <functional>

class QByteArray;
class QString;

//-------------------------------------------------------------------------------
// Usage: ReceiveVSSData( ... list & );
struct VSSRoutines
{
	class valid_parameters_count_sort_func : std::binary_function<CVSSData, CVSSData, bool> 
	{
	public:
		bool operator() (const CVSSData& _vssdatal, const CVSSData& _vssdatar) const
		{
			return ((CVSSData)_vssdatal).ValidParametersCount() > ((CVSSData)_vssdatar).ValidParametersCount();
		}
	};

   static bool ReceiveVSSDataCom(QString com, std::list<CVSSData>& _VSSDataPackets);
   static bool ReceiveVSSDataWeb(QString adr, std::list<CVSSData>& _VSSDataPackets);
   static void parseVSSData(std::list<CVSSData>& _VSSDataPackets, const QByteArray &buf, int &params_count, int &packets_count);

};

