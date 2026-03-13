#pragma once

#include "compute/Grid.hpp"
#include "domain/RiskEvent.hpp"
#include "domain/SectorSummary.hpp"
#include "domain/Track.hpp"
#include "domain/types/ProcessingResult.hpp"
#include "FlightData.pb.h"

RiskEventProto mapToProto(const RiskEvent &event);
SectorSummaryProto mapToProto(const SectorSummary &summary);
TrackProto mapToProto(const Track &track);

FlightDataProto mapToProto(const ProcessingResult &result, const Configuration &config,
                           const Grid &grid);