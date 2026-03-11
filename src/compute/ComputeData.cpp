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

ComputeData::ComputeData(Configuration configuration): 
config(configuration)
{
    initializeSectors();
}

void ComputeData::initializeSectors()
{
}

void ComputeData::handleTrackUpdate(const Track &newTrack)
{
    ProcessingResult result;
    int newSectorId = determineSector(newTrack.getPosition());
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
        int oldSectorId = determineSector(oldTrack.getPosition());
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
}

int ComputeData::determineSector(const Position &position)
{
}

void ComputeData::evaluateSectorState(int sectorId, std::int64_t timestamp)
{
    SectorSummary &summary = sectorSummariesById_.at(sectorId);

    SectorState oldState = summary.getState();

    summary.updateState();

    SectorState newState = summary.getState();

    if (oldState != newState)
    {
        RiskEvent riskEvent(sectorId, timestamp, newState);
        riskEvents_.push_back(riskEvent);
    }
}