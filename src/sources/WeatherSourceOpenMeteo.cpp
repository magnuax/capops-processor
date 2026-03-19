#include <algorithm>
#include <limits>
#include <stdexcept>
#include <vector>

#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>
#include <QStringList>

#include "sources/WeatherSourceOpenMeteo.hpp"

WeatherSourceOpenMeteo::WeatherSourceOpenMeteo()
{
    apiBaseUrl_ = "https://api.open-meteo.com/v1/forecast";
}

WeatherSeverity WeatherSourceOpenMeteo::getWeatherSeverity(Position coordinates)
{
    ApiResponse response = fetchWeatherData(coordinates);

    WeatherSeverity base = getSeverityFromWeatherCode(response.weatherCode);

    if (response.windGusts > 25.0)
        base = std::max(base, WeatherSeverity::DEGRADED);
    if (response.windGusts > 40.0)
        base = std::max(base, WeatherSeverity::SEVERE);
    if (response.windGusts > 60.0)
        base = std::max(base, WeatherSeverity::EXTREME);

    if (response.visibility < 5000)
        base = std::max(base, WeatherSeverity::DEGRADED);
    if (response.visibility < 1000)
        base = std::max(base, WeatherSeverity::SEVERE);
    if (response.visibility < 500)
        base = std::max(base, WeatherSeverity::EXTREME);

    return base;
}

WeatherSeverity WeatherSourceOpenMeteo::getSeverityFromWeatherCode(int code)
{
    if (code == 0 || code <= 3)
        return WeatherSeverity::OK;

    if (code == 45 || code == 48)
        return WeatherSeverity::DEGRADED; // fog

    if (code >= 51 && code <= 55)
        return WeatherSeverity::DEGRADED; // drizzle

    if (code >= 56 && code <= 57)
        return WeatherSeverity::DEGRADED; // freezing drizzle

    if (code >= 61 && code <= 63)
        return WeatherSeverity::DEGRADED; // rain

    if (code == 65)
        return WeatherSeverity::SEVERE; // heavy rain

    if (code >= 66 && code <= 67)
        return WeatherSeverity::SEVERE; // freezing rain

    if (code >= 71 && code <= 75)
        return WeatherSeverity::DEGRADED; // snow

    if (code >= 80 && code <= 82)
        return WeatherSeverity::SEVERE; // showers

    if (code >= 95 && code <= 99)
        return WeatherSeverity::EXTREME; // thunderstorm

    return WeatherSeverity::OK;
}

void WeatherSourceOpenMeteo::setRange(int range)
{
    currentRange_ = QString::number(range) + "m";
    setRange(currentRange_);
}

void WeatherSourceOpenMeteo::setRange(QString range)
{
    auto position = std::find(validRanges_.begin(), validRanges_.end(), range);

    if (position == validRanges_.end())
    {
        throw std::invalid_argument(
            "Invalid range format. Valid formats are: 10m, 80m, 120m, 180m.");
    }

    currentRange_ = range;
}

WeatherSourceOpenMeteo::ApiResponse WeatherSourceOpenMeteo::fetchWeatherData(Position coordinates)
{

    QString wind_speed = QString("wind_speed_%1").arg(currentRange_);
    QString wind_gusts = QString("wind_gusts_10");

    QString timezone = "auto";

    QStringList arguments = {"weather_code", "temperature_2m", wind_speed, wind_gusts,
                             "visibility",   "precipitation",  "snowfall"};

    QString requestURL = apiBaseUrl_ + "?latitude=" + QString::number(coordinates.latDeg) +
                         "&longitude=" + QString::number(coordinates.lonDeg) +
                         "&current=" + arguments.join(",") + "&timezone=" + timezone;

    QUrl url(requestURL);
    QEventLoop loop;

    QNetworkReply *reply = networkManager_.get(QNetworkRequest(url));

    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() != QNetworkReply::NoError)
    {
        reply->deleteLater();
        throw std::runtime_error(reply->errorString().toStdString());
    }

    QJsonDocument json = QJsonDocument::fromJson(reply->readAll());
    reply->deleteLater();

    ApiResponse response;

    response.timezone = json["timezone"].toString();
    response.timestamp = json["current"]["time"].toString();
    response.sampledLatitude = json["latitude"].toDouble();
    response.sampledLongitude = json["longitude"].toDouble();

    response.weatherCode = json["current"]["weather_code"].toInt();
    response.temperature = json["current"]["temperature_2m"].toDouble();
    response.windSpeed = json["current"][wind_speed].toDouble();
    response.windGusts = json["current"][wind_gusts].toDouble();
    response.visibility = json["current"]["visibility"].toDouble();
    response.precipitation = json["current"]["precipitation"].toDouble();
    response.snowfall = json["current"]["snowfall"].toDouble();

    return response;
}