#pragma once
#include "domain/RiskEvent.hpp"
#include "domain/SectorSummary.hpp"
#include "domain/Track.hpp"
#include <vector>

struct ProcessingResult
{
    std::vector<Track> trackUpdatesToPublish;
    std::vector<SectorSummary> sectorUpdatesToPublish;
    std::vector<RiskEvent> riskEventsToPublish;
};