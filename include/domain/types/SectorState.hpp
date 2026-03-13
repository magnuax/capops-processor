#pragma once
#include <string>

enum class SectorState
{
    NORMAL,
    CONGESTED,
    AT_RISK
};

std::string sectorStateToString(SectorState state);