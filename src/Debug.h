#pragma once

#include "PersistedData.h"

namespace Debug
{
	void DumpAllArousalData()
	{
		PersistedData::ArousalData::GetSingleton()->DumpToLog();
		PersistedData::ExposureData::GetSingleton()->DumpToLog();
		PersistedData::ArousalMultiplierData::GetSingleton()->DumpToLog();
		PersistedData::LastCheckTimeData::GetSingleton()->DumpToLog();
	}

	void ClearAllArousalData()
	{
		PersistedData::ArousalData::GetSingleton()->Clear();
		PersistedData::ExposureData::GetSingleton()->Clear();
		PersistedData::ArousalMultiplierData::GetSingleton()->Clear();
		PersistedData::LastCheckTimeData::GetSingleton()->Clear();
	}
}
