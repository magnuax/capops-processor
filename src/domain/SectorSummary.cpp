#include "domain/SectorSummary.hpp"

SectorSummary::SectorSummary(std::string sectorId, std::int64_t timestamp, int trackCount,
                             WeatherSeverity weatherSeverity, double weatherFactor,
                             double baseCapacity, double effectiveCapacity, SectorState state)
    : sectorId_(std::move(sectorId)), timestamp_(timestamp), trackCount_(trackCount),
      weatherSeverity_(weatherSeverity), weatherFactor_(weatherFactor), baseCapacity_(baseCapacity),
      effectiveCapacity_(effectiveCapacity), state_(state)
{
}

int SectorSummary::getTrackCount() 
{
    return trackCount_;
}

double SectorSummary::getBaseCapacity() 
{
    return baseCapacity_;
}

void SectorSummary::increaseTrackCount()
{
    trackCount_++;
}

void SectorSummary::updateState(SectorState newState)
{
    state_ = newState;
}

SectorState SectorSummary::getState() 
{
    return state_;
}
double SectorSummary::getEffectiveCapacity() 
{
    return effectiveCapacity_;
}