#include "domain/SectorSummary.hpp"

SectorSummary::SectorSummary(int sectorId, std::int64_t timestamp, int trackCount,
                             WeatherSeverity weatherSeverity, double weatherFactor,
                             double baseCapacity, double effectiveCapacity, SectorState state)
    : sectorId_(sectorId), timestamp_(timestamp), trackCount_(trackCount),
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

SectorState SectorSummary::getState()
{
    return state_;
}
double SectorSummary::getEffectiveCapacity()
{
    return effectiveCapacity_;
}

void SectorSummary::decreaseTrackCount()
{
    if (trackCount_ > 0)
    {
        trackCount_--;
    }
}

bool SectorSummary::isAtRisk()
{
    if (trackCount_ > effectiveCapacity_)
    {
        return true;
    }
    return false;
}

bool SectorSummary::isCongested()
{
    if (trackCount_ > baseCapacity_)
    {
        return true;
    }
    return false;
}

void SectorSummary::updateState()
{
    if (isAtRisk())
    {
        state_ = SectorState::AT_RISK;
    }
    else if (isCongested())
    {
        state_ = SectorState::CONGESTED;
    }
    else
    {
        state_ = SectorState::NORMAL;
    }
}

void SectorSummary::updateTime(std::int64_t timestamp){
  timestamp_ = timestamp; 
}
