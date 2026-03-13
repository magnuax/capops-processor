#pragma once
#include "domain/types/SectorState.hpp"
#include <cstdint>
#include <string>

class RiskEvent
{
  public:
    RiskEvent(int riskEventId, SectorState riskSeverity, int sectorId, std::string createdTimestamp,
              std::string message);

    int getSectorId() const;
    std::string getTimestamp() const;
    SectorState getState() const;
    int getRiskEventId() const;
    std::string getMessage() const; 

  private:
    int riskEventId_;
    SectorState riskSeverity_;
    int sectorId_;
    std::string createdTimestamp_;
    std::string message_;
};