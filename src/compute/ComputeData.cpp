#include "compute/ComputeData.hpp"
#include "domain/RiskEvent.hpp"
#include "domain/SectorSummary.hpp"
#include "domain/Track.hpp"
#include "domain/WeatherCell.hpp"
#include "domain/types/Position.hpp"
#include "domain/types/ProcessingResult.hpp"
#include <deque>
#include <string>
#include <unordered_map>


ComputeData::ComputeData(const Configuration& configuration)
    : config_(configuration), 
    grid_(configuration.grid())
{
    initializeSectors();
}

void ComputeData::initializeSectors()
{
    for (int sectorId = 0; sectorId < grid_.sectorCount(); ++sectorId) {
        sectorSummariesById_.emplace(
            sectorId,
            SectorSummary(
                sectorId,
                0,                      // timestamp
                0,                      // trackCount
                WeatherSeverity::OK,
                1.0,                    // weatherFactor
                config_.defaultBaseCapacity(),
                SectorState::NORMAL));
    }
}

void ComputeData::handleTrackUpdate(const Track &newTrack)
{
    int newSectorId = grid_.determineSector(newTrack.getPosition());
    auto currentTrack = activeTracksByIcao_.find(newTrack.getIcao());
    std::int64_t time = newTrack.getTimestamp();

    // existing track update
    if (currentTrack != activeTracksByIcao_.end())
    {
        const Track &oldTrack = currentTrack->second;
        if (newTrack.getTimestamp() <= oldTrack.getTimestamp())
        {
            return;
        }
        int oldSectorId = grid_.determineSector(oldTrack.getPosition());
        if (oldSectorId != newSectorId)
        {
            sectorSummariesById_.at(oldSectorId).decreaseTrackCount();
            evaluateSectorState(oldSectorId, time);

            sectorSummariesById_.at(newSectorId).increaseTrackCount();
            evaluateSectorState(newSectorId, time);
        }
        currentTrack->second = newTrack;
    }

    // new track
    else
    {
        sectorSummariesById_.at(newSectorId).increaseTrackCount();
        evaluateSectorState(newSectorId, time);
        activeTracksByIcao_.insert({newTrack.getIcao(), newTrack});
    }
}

void ComputeData::handleWeatherUpdate(const WeatherCell &weatherCell)
{
    int sectorId = weatherCell.getSectorId(); 
    WeatherSeverity severity = weatherCell.getWeatherSeverity();
    double factor = config_.weatherFactor(severity);
    SectorSummary &summary = sectorSummariesById_.at(sectorId);

    summary.updateWeather(weatherCell.getWeatherSeverity(), factor); 
    
    evaluateSectorState(sectorId, weatherCell.getTimestamp());

}



void ComputeData::evaluateSectorState(int sectorId, std::int64_t timestamp)
{
    SectorSummary &summary = sectorSummariesById_.at(sectorId);

    SectorState oldState = summary.getState();

    summary.updateState();

    summary.updateTime(timestamp); 

    SectorState newState = summary.getState();

    if (oldState != newState)
    {
        totalRiskEvents_++; 
        RiskEvent riskEvent(totalRiskEvents_, sectorId, timestamp, newState);
        pendingRiskEvents_.push_back(riskEvent);
    }
}


ProcessingResult ComputeData::collectDataForPublish()
{
    ProcessingResult result;

    for (const auto& [icao, track] : activeTracksByIcao_) {
        result.trackUpdatesToPublish.push_back(track);
    }

    for (const auto& [sectorId, summary] : sectorSummariesById_) {
        result.sectorUpdatesToPublish.push_back(summary);
    }

    while (!pendingRiskEvents_.empty()) {
        result.riskEventsToPublish.push_back(pendingRiskEvents_.front());
        pendingRiskEvents_.pop_front();
    }

    return result;
}