#include "domain/types/SectorState.hpp"

std::string sectorStateToString(SectorState state)
{
    switch (state)
    {
    case SectorState::NORMAL:
        return "NORMAL";
    case SectorState::CONGESTED:
        return "CONGESTED";
    case SectorState::AT_RISK:
        return "AT_RISK";
    }
    return "UNKNOWN";
}