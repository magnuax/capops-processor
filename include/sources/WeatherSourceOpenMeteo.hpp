#pragma once
#include "sources/interfaces/IWeatherSource.hpp"

class WeatherSourceOpenMeteo : public IWeatherSource
{
  public:
    WeatherSourceOpenMeteo();

    WeatherSeverity getWeatherSeverity(Position coordinates) override;

    void setRange(int range);
    void setRange(QString range);

  private:
    struct ApiResponse
    {
        // metadata
        QString timezone;
        QString timestamp;
        double sampledLatitude;
        double sampledLongitude;

        // severity parameters
        int weatherCode;
        double temperature;
        double windSpeed;
        double windGusts;
        double visibility;
        double precipitation;
        double snowfall;
    };

    QNetworkAccessManager networkManager_;

    QString apiBaseUrl_;
    ApiResponse apiResponse_;

    std::vector<QString> validRanges_ = {"10m", "80m", "120m", "180m"};
    QString currentRange_ = "80m";

    ApiResponse fetchWeatherData(Position coordinates);

    WeatherSeverity getSeverityFromWeatherCode(int code);
};