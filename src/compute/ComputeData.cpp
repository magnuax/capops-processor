#include "compute/ComputeData.hpp"
#inlcude "domain/Track.hpp"
#include "domain/WeatherCell.hpp"
#include "domain/SectorSummary.hpp"
#include "domain/RiskEvent.hpp"
#include "domain/types/ProcessingResult.hpp"
#include "domain/types/Position.hpp"


ComputeData::ComputeData() {
    // Initialize sector summaries

}

ProcessingResult ComputeData::handleTrackUpdate(const Track &track)
{
    if (activeTracksByIcao_.find(track.getIcao()) != activeTracksByIcao_.end())
    {
        
        activeTracksByIcao_[track.getIcao()] = track;
    }
    else
    {
        Position trackPosition = track.getPosition();
        int sectorId = determineSector(trackPosition);
        sectorSummariesById_[sectorId].increaseTrackCount();
        handleRiskEvent(sectorId); 

        }
    }

}
ProcessingResult ComputeData::handleWeatherUpdate(const WeatherCell &weatherCell)
{
    
}

int ComputeData::determineSector(Position &position)
{
    
}

void ComputeData::handleRiskEvent(int sectorId, std::int64_t timestamp)
{
    if(sectorSummariesById_[sectorId].getState() == SectorState::AT_RISK){
        return; 
    }
    else if (isAtRisk(sectorId)){
        RiskEvent riskEvent(sectorId, timestamp, SectorState::AT_RISK);
        riskEvents_.push_back(riskEvent);
        return;
    }
    else{
        if(isCongested(sectorId)){
            RiskEvent riskEvent(sectorId, timestamp, SectorState::CONGESTED);
            riskEvents_.push_back(riskEvent);
        }
    }
}


bool ComputeData::isAtRisk(int sectorId){
    if (sectorSummariesById_[sectorId].getTrackCount() > sectorSummariesById_[sectorId].getEffectiveCapacity() ){
        sectorSummariesById_[sectorId].updateState(SectorState::AT_RISK); 
        return true;
    }
    return false; 
}

bool ComputeData::isCongested(int sectorId){
    if (sectorSummariesById_[sectorId].getTrackCount() > sectorSummariesById_[sectorId].getBaseCapacity() ){
        sectorSummariesById_[sectorId].updateState(SectorState::CONGESTED); 
        return true;
    }
    return false; 
}
