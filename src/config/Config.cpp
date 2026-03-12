#include "config/Config.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>


Configuration::Configuration(const std::string& path)
{
    load(path);
}


const GridConfig &Configuration::grid() const

{
    return grid_;
}

double Configuration::defaultBaseCapacity() const
{
    return defaultBaseCapacity_;
}

double Configuration::weatherFactor(WeatherSeverity severity) const
{
    return weatherFactors_.at(severity);
}




void Configuration::load(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open config file");
    }

    std::string line;
    std::string currentSection;

    while (std::getline(file, line))
    {

        if (line.empty()) continue;
        if (line[0] == '[')
        {
            currentSection = line.substr(1, line.size() - 2);
            continue;
        }

        auto pos = line.find('=');

        if (pos == std::string::npos) continue;

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        if (currentSection == "grid")
        {

            if (key == "minLat") grid_.minLat = std::stod(value);
            else if (key == "maxLat") grid_.maxLat = std::stod(value);
            else if (key == "minLon") grid_.minLon = std::stod(value);
            else if (key == "maxLon") grid_.maxLon = std::stod(value);
            else if (key == "rows") grid_.rows = std::stoi(value);
            else if (key == "cols") grid_.cols = std::stoi(value);

        }

        else if (currentSection == "capacity")
        {
            if (key == "defaultBaseCapacity")
                defaultBaseCapacity_ = std::stod(value);
        }


        else if (currentSection == "weatherFactors")
        {
            if (key == "OK")
                weatherFactors_[WeatherSeverity::OK] = std::stod(value);
            else if (key == "DEGRADED")
                weatherFactors_[WeatherSeverity::DEGRADED] = std::stod(value);
            else if (key == "SEVERE")
                weatherFactors_[WeatherSeverity::SEVERE] = std::stod(value);
            else if (key == "EXTREME")
                weatherFactors_[WeatherSeverity::EXTREME] = std::stod(value);
        }
    }
}