#include "domain/RiskEvent.hpp"

RiskEvent::RiskEvent(int riskEventId, SectorState riskSeverity, int sectorId,
                     std::string createdTimestamp, std::string message)
    : riskEventId_(std::move(riskEventId)), riskSeverity_(riskSeverity),
      sectorId_(std::move(sectorId)), createdTimestamp_(createdTimestamp), message_(message)
{
}

int RiskEvent::getSectorId() const
{
    return sectorId_;
}

std::string RiskEvent::getTimestamp() const
{
    return createdTimestamp_;
}

SectorState RiskEvent::getState() const
{
    return riskSeverity_;
}

int RiskEvent::getRiskEventId() const{
    return riskEventId_;
}

std::string RiskEvent::getMessage() const{
    return message_; 
}