#include "domain/RiskEvent.hpp"

RiskEvent::RiskEvent(int sectorId, std::int64_t timestamp, SectorState state)
    : sectorId_(std::move(sectorId)), timestamp_(timestamp), state_(state)
{
}