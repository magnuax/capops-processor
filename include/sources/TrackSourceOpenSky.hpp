#pragma once

#include <QNetworkAccessManager>
#include <map>

#include "config/Config.hpp"
#include "sources/interfaces/ITrackSource.hpp"

class TrackSourceOpenSky : public ITrackSource
{
  public:
    TrackSourceOpenSky();

    std::optional<Track> getTrack(const std::string &icao) const override;

    std::vector<Track> getAllTracks() const override;

    void setRegion(GridConfig config);
    void setRegion(double latMin, double latMax, double lonMin, double lonMax);

  private:
    struct ApiResponse
    {
        std::string timestamp;

        std::string icao24;
        std::string callsign;

        double latitude;
        double longitude;
        double baroAltitude;
        bool onGround;
        double velocity;
        double snowfall;
        double trueTrack;
        double verticalRate;
    };

    QNetworkAccessManager *networkManager_;

    double minLat_ = 0;
    double maxLat_ = 0;
    double minLon_ = 0;
    double maxLon_ = 0;

    QByteArray blockingGet(const QString &url) const;

    static ApiResponse parseStates(QJsonArray &states);
};