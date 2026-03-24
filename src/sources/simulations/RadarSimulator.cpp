#include "sources/simulations/RadarSimulator.hpp"
#include <cmath>
#include <random>
#include <stdexcept>

RadarSimulator::RadarSimulator(GridConfig config)
{
    timeStep_ = 1.0;
    currentTime_ = 0.0;
    gridConfig_ = config;
}

void RadarSimulator::initializeFlights(int numFlights)
{
    std::random_device device;
    std::mt19937 generator(device());

    std::uniform_real_distribution<double> dist(0, 1);

    double minLat = gridConfig_.minLat;
    double minLon = gridConfig_.minLon;
    double latWidth = gridConfig_.maxLat - gridConfig_.minLat;
    double lonWidth = gridConfig_.maxLon - gridConfig_.minLon;

    std::vector<FlightPosition> startPositions;
    std::vector<FlightPosition> endPositions;
    std::vector<double> speeds;

    for (int i = 0; i < numFlights; ++i)
    {
        double latStart = minLat + latWidth * dist(generator);
        double lonStart = minLon + lonWidth * dist(generator);

        double latEnd = minLat + latWidth * dist(generator);
        double lonEnd = minLon + lonWidth * dist(generator);

        double speed = 200 + 100 * dist(generator); // ~390–580 kts

        startPositions.push_back({latStart, lonStart});
        endPositions.push_back({latEnd, lonEnd});
        speeds.push_back(speed);
    }

    initializeFlights(startPositions, endPositions, speeds);
}

void RadarSimulator::initializeFlights(std::vector<FlightPosition> startPositions,
                                       std::vector<FlightPosition> endPositions,
                                       std::vector<double> speeds)
{
    int numStartPositions = startPositions.size();
    int numEndPositions = endPositions.size();
    int numVelocities = speeds.size();

    if (!(numStartPositions == numEndPositions && numStartPositions == numVelocities))
    {
        throw std::invalid_argument("Input vectors must have the same size");
    }

    for (size_t i = 0; i < startPositions.size(); ++i)
    {
        FlightID icao = "SIM-" + std::to_string(i);

        flightIds_.push_back(icao);
        flightPositions_[icao] = startPositions[i];

        double dlat = endPositions[i].first - startPositions[i].first;
        double dlon = endPositions[i].second - startPositions[i].second;

        // Convert roughly to Cartesian distance
        double jacobianLat = 111.32e3;
        double jacobianLon = 111.32e3 * std::cos(startPositions[i].first * M_PI / 180.0);

        double dx = dlat * jacobianLat;
        double dy = dlon * jacobianLon;

        double dr = std::sqrt(dx * dx + dy * dy);
        double dt = dr / speeds[i];

        double vLat = dx / dt / jacobianLat;
        double vLon = dy / dt / jacobianLon;

        flightVelocities_[icao] = {vLat, vLon};
    }

    initialized = true;
}

void RadarSimulator::start()
{

    if (!initialized)
    {
        int numFlights = 20;
        initializeFlights(numFlights);
    }

    isRunning_ = true;

    while (isRunning_)
        tick(timeStep_);
}

void RadarSimulator::stop()
{
    isRunning_ = false;
}

void RadarSimulator::tick(double timeStep)
{
    currentTime_ += timeStep;

    double latWidth = gridConfig_.maxLat - gridConfig_.minLat;
    double lonWidth = gridConfig_.maxLon - gridConfig_.minLon;

    for (const auto &flight : flightIds_)
    {
        FlightPosition pos = flightPositions_.at(flight);
        FlightVelocity vel = flightVelocities_.at(flight);

        double newLat = pos.first + vel.first * timeStep;
        double newLon = pos.second + vel.second * timeStep;

        FlightPosition newPos = enforcePeriodicBoundary({newLat, newLon}, latWidth, lonWidth);

        flightPositions_[flight] = newPos;
    }
}

RadarSimulator::FlightPosition
RadarSimulator::enforcePeriodicBoundary(FlightPosition pos, double latWidth, double lonWidth)
{
    double newLat = pos.first;
    double newLon = pos.second;

    newLat = gridConfig_.minLat + std::fmod(newLat - gridConfig_.minLat, latWidth);
    if (newLat < gridConfig_.minLat)
        newLat += latWidth;

    newLon = gridConfig_.minLon + std::fmod(newLon - gridConfig_.minLon, lonWidth);
    if (newLon < gridConfig_.minLon)
        newLon += lonWidth;

    return {newLat, newLon};
}

bool RadarSimulator::containsFlight(const FlightID &flightId) const
{
    return flightPositions_.find(flightId) != flightPositions_.end();
}

std::vector<RadarSimulator::FlightID> RadarSimulator::getFlights() const
{
    return flightIds_;
}

RadarSimulator::FlightPosition RadarSimulator::getPosition(const FlightID &flightId) const
{
    return flightPositions_.at(flightId);
}

RadarSimulator::FlightVelocity RadarSimulator::getVelocity(const FlightID &flightId) const
{
    return flightVelocities_.at(flightId);
}

double RadarSimulator::getSpeed(const FlightID &flightId) const
{
    auto vel = flightVelocities_.at(flightId);
    return std::sqrt(vel.first * vel.first + vel.second * vel.second);
}