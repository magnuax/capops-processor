#pragma once
#include "domain/types/SectorState.hpp"
#include <cstdint>
#include <string>

struct RiskEventPayload
{
    int riskEventId;
    std::string riskSeverity;
    int sectorId;
    std::string createdTimestamp;
    std::string message;
    bool acknowledged;
    std::string acknowledgedTimeStamp;
};

class RiskEvent
{
  public:
    RiskEvent(int riskEventId, SectorState riskSeverity, int sectorId, std::string createdTimestamp,
              std::string message);

    RiskEventPayload toPayload() const;
    int getSectorId() const;
    std::string getTimestamp() const;
    SectorState getState() const;

  private:
    int riskEventId_;
    SectorState riskSeverity_;
    int sectorId_;
    std::string createdTimestamp_;
    std::string message_;
};