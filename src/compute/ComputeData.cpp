#include "compute/ComputeData.hpp"
#include "domain/RiskEvent.hpp"
#include "domain/SectorSummary.hpp"
#include "domain/Track.hpp"
#include "domain/WeatherCell.hpp"
#include "domain/types/Position.hpp"
#include "domain/types/ProcessingResult.hpp"
#include <deque>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace
{
int riskLevel(SectorState state)
{
    switch (state)
    {
    case SectorState::NORMAL:
        return 0;
    case SectorState::CONGESTED:
        return 1;
    case SectorState::AT_RISK:
        return 2;
    }

    throw std::runtime_error("Unknown SectorState");
}

bool isEscalation(SectorState oldState, SectorState newState)
{
    return riskLevel(newState) > riskLevel(oldState);
}

bool isDeescalation(SectorState oldState, SectorState newState)
{
    return riskLevel(newState) < riskLevel(oldState);
}

std::string buildRiskEventMessage(int sectorId, SectorState oldState, SectorState newState)
{
    if (isEscalation(oldState, newState))
    {
        return "Sector " + std::to_string(sectorId) + " risk level escalated from " +
               sectorStateToString(oldState) + " to " + sectorStateToString(newState) + ".";
    }

    if (isDeescalation(oldState, newState))
    {
        return "Sector " + std::to_string(sectorId) + " risk level de-escalated from " +
               sectorStateToString(oldState) + " to " + sectorStateToString(newState) + ".";
    }

    return "Sector " + std::to_string(sectorId) + " state unchanged at " +
           sectorStateToString(newState) + ".";
}
} // namespace

ComputeData::ComputeData(const Configuration &configuration)
    : config_(configuration), grid_(configuration.grid())
{
    initializeSectors();
}

void ComputeData::initializeSectors()
{
    for (int sectorId = 0; sectorId < grid_.sectorCount(); ++sectorId)
    {
        sectorSummariesById_.emplace(
            sectorId, SectorSummary(sectorId, grid_.row(sectorId), grid_.column(sectorId),
                                    "", // timestamp
                                    0,  // localAircraftCount
                                    WeatherSeverity::OK,
                                    1.0, // weatherFactor
                                    config_.defaultBaseCapacity(), SectorState::NORMAL));
    }
}

void ComputeData::handleTrackUpdate(const Track &newTrack)
{
    int newSectorId = grid_.determineSector(newTrack.getPosition());
    auto currentTrack = activeTracksByIcao_.find(newTrack.getIcao());
    std::string time = newTrack.getTimestamp();

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
            sectorSummariesById_.at(oldSectorId).decreaseLocalAircraftCount();
            evaluateSectorState(oldSectorId, time);

            sectorSummariesById_.at(newSectorId).increaseLocalAircraftCount();
            evaluateSectorState(newSectorId, time);
        }
        currentTrack->second = newTrack;
    }

    // new track
    else
    {
        sectorSummariesById_.at(newSectorId).increaseLocalAircraftCount();
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

void ComputeData::evaluateSectorState(int sectorId, const std::string &timestamp)
{
    SectorSummary &summary = sectorSummariesById_.at(sectorId);

    SectorState oldState = summary.getState();

    summary.updateState();

    summary.updateTime(timestamp);

    SectorState newState = summary.getState();

    if (oldState != newState)
    {
        totalRiskEvents_++;
        std::string message = buildRiskEventMessage(sectorId, oldState, newState);

        RiskEvent riskEvent(totalRiskEvents_, newState, sectorId, timestamp, message);
        pendingRiskEvents_.push_back(riskEvent);
    }
}

ProcessingResult ComputeData::collectProcessingResult()
{
    ProcessingResult result;

    for (const auto &[icao, track] : activeTracksByIcao_)
    {
        result.tracks.push_back(track);
    }

    for (const auto &[sectorId, summary] : sectorSummariesById_)
    {
        result.sectorSummaries.push_back(summary);
    }

    while (!pendingRiskEvents_.empty())
    {
        result.riskEvents.push_back(pendingRiskEvents_.front());
        pendingRiskEvents_.pop_front();
    }

    return result;
}
