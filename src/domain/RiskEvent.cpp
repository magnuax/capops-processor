#include "domain/RiskEvent.hpp"

RiskEvent::RiskEvent(int sectorId, std::int64_t timestamp, SectorState state)
    : sectorId_(std::move(sectorId)), timestamp_(timestamp), state_(state)
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
