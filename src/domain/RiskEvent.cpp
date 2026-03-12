#include "domain/RiskEvent.hpp"

RiskEvent::RiskEvent(int riskEventId, int sectorId, std::int64_t timestamp, SectorState state)
    : riskEventId_(std::move(riskEventId)), sectorId_(std::move(sectorId)), timestamp_(timestamp), state_(state)
{
}

int RiskEvent::getSectorId() const{
    return sectorId_; 
}

std::int64_t RiskEvent::getTimestamp() const{
    return timestamp_; 
}


SectorState RiskEvent::getState() const{
    return state_; 
}
