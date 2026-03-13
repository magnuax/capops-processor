#include "publish/ProtoMapper.hpp"

#include "domain/types/SectorState.hpp"
#include "domain/types/WeatherSeverity.hpp"
#include "utils/time/IsoTimestamp.hpp"

#include <string>

RiskEventProto mapToProto(const RiskEvent &event)
{
    RiskEventProto proto;

    proto.set_riskeventid(event.getRiskEventId());
    proto.set_riskseverity(sectorStateToString(event.getState()));
    proto.set_sectorid(event.getSectorId());
    proto.set_createdtimestamp(event.getTimestamp());
    proto.set_acknowledgedtimestamp("");
    proto.set_message(event.getMessage());
    proto.set_acknowledged(false);

    return proto;
}

SectorSummaryProto mapToProto(const SectorSummary &summary)
{
    SectorSummaryProto proto;

    proto.set_sectorid(summary.getSectorId());
    proto.set_row(summary.getRow());
    proto.set_column(summary.getColumn());
    proto.set_weatherseverity(weatherSeverityToString(summary.getWeatherSeverity()));
    proto.set_riskseverity(sectorStateToString(summary.getState()));
    proto.set_localaircraftcount(summary.getLocalAircraftCount());
    proto.set_localaircraftbasecapacity(static_cast<int>(summary.getBaseCapacity()));
    proto.set_localaircrafteffectivecapacity(static_cast<int>(summary.getEffectiveCapacity()));

    return proto;
}

TrackProto mapToProto(const Track &track)
{
    TrackProto proto;

    proto.set_icao24(track.getIcao());
    proto.set_timestamp(track.getTimestamp());
    proto.set_headingdegrees(track.getHeadingDegrees());
    proto.set_groundtrackdegrees(track.getGroundTrackDegrees());

    PositionProto *position = proto.mutable_position();
    position->set_latitudedegrees(track.getPosition().latDeg);
    position->set_longitudedegrees(track.getPosition().lonDeg);
    position->set_altitudemeters(track.getAltitudeMeters());

    VelocityProto *velocity = proto.mutable_velocity();
    velocity->set_groundspeedknots(track.getGroundSpeedKnots());
    velocity->set_verticalspeedfeetperminute(track.getVerticalSpeedFeetPerMinute());

    return proto;
}

FlightDataProto mapToProto(const ProcessingResult &result, const Configuration &config,
                           const Grid &grid)
{
    FlightDataProto proto;

    // Metadata
    proto.mutable_metadata()->set_version(config.getProtobufVersion());
    proto.mutable_metadata()->set_timestamp(createIsoTimestamp());

    // Risk events
    RiskEventDataProto* riskEventData = proto.mutable_riskeventdata();
    riskEventData>set_riskeventcount(static_cast<int>(result.riskEvents.size()));

    
    for (const auto &event : result.riskEvents)
    {
        *riskEventData->add_riskevents() = mapToProto(event);
    }

    // Sector summaries
    SectorSummaryDataProto *sectorSummaryData = proto.mutable_sectorsummarydata();
    sectorSummaryData->set_rowscount(grid.rows());
    sectorSummaryData->set_columnscount(grid.cols());
    sectorSummaryData->set_minlongitude(config.grid().minLon);
    sectorSummaryData->set_maxlongitude(config.grid().maxLon);
    sectorSummaryData->set_minlatitude(config.grid().minLat);
    sectorSummaryData->set_maxlatitude(config.grid().maxLat);

    for (const auto &summary : result.sectorSummaries)
    {
        *sectorSummaryData->add_sectorsummaries() = mapToProto(summary);
    }

    // Tracks
    TrackDataProto *trackData = proto.mutable_trackdata();
    trackData->set_totalaircraftscount(static_cast<int>(result.tracks.size()));
    trackData->set_coordinatesystem(config.getCoordinateSystem());

    for (const auto &track : result.tracks)
    {
        *trackData->add_tracks() = mapToProto(track);
    }

    return proto;
}