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

RiskEventPayload RiskEvent::toPayload() const
{
    return RiskEventPayload{riskEventId_,
                            sectorStateToString(riskSeverity_),
                            sectorId_,
                            createdTimestamp_,
                            message_,
                            false,
                            0};
}