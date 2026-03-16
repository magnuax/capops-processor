#include "domain/SectorSummary.hpp"

SectorSummary::SectorSummary(int sectorId, int row, int column, std::string timestamp,
                             int localAircraftCount, WeatherSeverity weatherSeverity,
                             double weatherFactor, double localAircraftBaseCapacity,
                             SectorState riskSeverity)
    : sectorId_(sectorId), row_(row), column_(column), timestamp_(std::move(timestamp)),
      localAircraftCount_(localAircraftCount), weatherSeverity_(weatherSeverity),
      weatherFactor_(weatherFactor), localAircraftBaseCapacity_(localAircraftBaseCapacity),
      riskSeverity_(riskSeverity)
{
}

int SectorSummary::getSectorId() const
{
    return sectorId_;
}

int SectorSummary::getRow() const
{
    return row_;
}

int SectorSummary::getColumn() const
{
    return column_;
}

int SectorSummary::getLocalAircraftCount() const
{
    return localAircraftCount_;
}

double SectorSummary::getBaseCapacity() const
{
    return localAircraftBaseCapacity_;
}

void SectorSummary::increaseLocalAircraftCount()
{
    localAircraftCount_++;
}

SectorState SectorSummary::getState() const
{
    return riskSeverity_;
}

double SectorSummary::getEffectiveCapacity() const
{
    return localAircraftBaseCapacity_ * weatherFactor_;
}

void SectorSummary::decreaseLocalAircraftCount()
{
    if (localAircraftCount_ > 0)
    {
        localAircraftCount_--;
    }
}

bool SectorSummary::isAtRisk()
{
    if (localAircraftCount_ > getEffectiveCapacity())
    {
        return true;
    }
    return false;
}

bool SectorSummary::isCongested()
{
    if (localAircraftCount_ > localAircraftBaseCapacity_)
    {
        return true;
    }
    return false;
}

void SectorSummary::updateState()
{
    if (isAtRisk())
    {
        riskSeverity_ = SectorState::AT_RISK;
    }
    else if (isCongested())
    {
        riskSeverity_ = SectorState::CONGESTED;
    }
    else
    {
        riskSeverity_ = SectorState::NORMAL;
    }
}

void SectorSummary::updateTime(std::string timestamp)
{
    timestamp_ = std::move(timestamp);
}

void SectorSummary::updateWeather(WeatherSeverity weatherSeverity, double weatherFactor)
{
    weatherSeverity_ = weatherSeverity;
    weatherFactor_ = weatherFactor;
}

WeatherSeverity SectorSummary::getWeatherSeverity() const
{
    return weatherSeverity_;
}
