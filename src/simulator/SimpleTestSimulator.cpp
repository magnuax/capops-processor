#include "simulator/SimpleTestSimulator.hpp"

#include "domain/types/Position.hpp"
#include "domain/types/WeatherSeverity.hpp"

#include <stdexcept>

SimpleTestSimulator::SimpleTestSimulator()
{
    // ---------------------------
    // FASE 1: Rolig start
    // ---------------------------
    events_.push_back(Track("ABC123", "1000", Position{59.10, 4.10}, 10000.0, 230.0, 90.0, 90, 90));
    // events_.push_back(Track("DEF456", 1001, Position{59.20, 4.20}, 10500.0, 240.0, 95.0));
    // events_.push_back(Track("GHI789", 1002, Position{60.10, 5.10}, 11000.0, 250.0, 100.0));
    // events_.push_back(WeatherCell(1, 1003, WeatherSeverity::OK));
    events_.push_back(WeatherCell(2, "1004", WeatherSeverity::OK));

    // ---------------------------
    // FASE 2: Flere tracks inn i sektor 1
    // Kan gi endret sector summary
    // ---------------------------
    // events_.push_back(Track("JKL321", "2000", Position{59.11, 4.11}, 9800.0, 220.0, 85.0));
    // events_.push_back(Track("MNO654", 2001, Position{59.12, 4.12}, 9700.0, 210.0, 80.0));
    // events_.push_back(Track("PQR987", 2002, Position{59.13, 4.13}, 9600.0, 215.0, 82.0));

    // // Oppdater eksisterende fly
    // events_.push_back(Track("ABC123", 2003, Position{59.14, 4.14}, 10050.0, 231.0, 91.0));
    // events_.push_back(Track("DEF456", 2004, Position{59.21, 4.21}, 10520.0, 242.0, 96.0));
    // events_.push_back(Track("GHI789", 2005, Position{60.11, 5.11}, 11020.0, 251.0, 101.0));

    // // ---------------------------
    // // FASE 3: Været forverres i sektor 1
    // // Bør kunne påvirke risk events
    // // ---------------------------
    // events_.push_back(WeatherCell(1, 3000, WeatherSeverity::SEVERE));
    // events_.push_back(Track("STU111", 3001, Position{59.15, 4.15}, 9400.0, 205.0, 78.0));
    // events_.push_back(Track("VWX222", 3002, Position{59.16, 4.16}, 9300.0, 200.0, 75.0));

    // // ---------------------------
    // // FASE 4: SEVERE vær i sektor 1
    // // Mange fly i samme sektor + severe weather
    // // Dette er en klassisk risk event-test
    // // ---------------------------
    // events_.push_back(WeatherCell(1, 4000, WeatherSeverity::SEVERE));
    // events_.push_back(Track("ABC123", 4001, Position{59.17, 4.17}, 10080.0, 232.0, 92.0));
    // events_.push_back(Track("DEF456", 4002, Position{59.18, 4.18}, 10540.0, 243.0, 97.0));
    // events_.push_back(Track("JKL321", 4003, Position{59.19, 4.19}, 9820.0, 221.0, 86.0));
    // events_.push_back(Track("MNO654", 4004, Position{59.20, 4.20}, 9720.0, 211.0, 81.0));

    // // ---------------------------
    // // FASE 5: Økt aktivitet i sektor 2
    // // Tester at du håndterer flere sektorer samtidig
    // // ---------------------------
    // events_.push_back(Track("YYY333", 5000, Position{60.20, 5.20}, 11200.0, 255.0, 102.0));
    // events_.push_back(Track("ZZZ444", 5001, Position{60.21, 5.21}, 11300.0, 256.0, 103.0));
    // events_.push_back(Track("AAA555", 5002, Position{60.22, 5.22}, 11400.0, 257.0, 104.0));
    // events_.push_back(WeatherCell(2, 5003, WeatherSeverity::SEVERE));

    // // ---------------------------
    // // FASE 6: Vær bedres i sektor 1
    // // Burde kunne gi nytt risk event eller state-endring
    // // ---------------------------
    // events_.push_back(WeatherCell(1, 6000, WeatherSeverity::OK));
    // events_.push_back(Track("ABC123", 6001, Position{59.25, 4.25}, 10100.0, 233.0, 93.0));
    // events_.push_back(Track("DEF456", 6002, Position{59.26, 4.26}, 10560.0, 244.0, 98.0));

    // ---------------------------
    // FASE 7: Sektor 3 dukker opp med ekstrem situasjon
    // Tester ny sektor + severe direkte
    // // ---------------------------
    // events_.push_back(WeatherCell(3, 7000, WeatherSeverity::SEVERE));
    // events_.push_back(Track("BBB666", 7001, Position{61.10, 6.10}, 9000.0, 190.0, 70.0));
    // events_.push_back(Track("CCC777", 7002, Position{61.11, 6.11}, 9100.0, 191.0, 71.0));
    // events_.push_back(Track("DDD888", 7003, Position{61.12, 6.12}, 9200.0, 192.0, 72.0));
    // events_.push_back(Track("EEE999", 7004, Position{61.13, 6.13}, 9300.0, 193.0, 73.0));

    // // ---------------------------
    // // FASE 8: Edge cases
    // // ---------------------------

    // // Gammel track-update for samme fly (burde kanskje ignoreres hvis du støtter det)
    // events_.push_back(Track("ABC123", 1500, Position{58.00, 3.00}, 8000.0, 100.0, 45.0));

    // // Duplikat / nesten identisk oppdatering
    // events_.push_back(Track("EEE999", 7004, Position{61.13, 6.13}, 9300.0, 193.0, 73.0));

    // // Vær skifter igjen i sektor 2
    // events_.push_back(WeatherCell(2, 8000, WeatherSeverity::SEVERE));
    // events_.push_back(WeatherCell(2, 9000, WeatherSeverity::OK));
}
bool SimpleTestSimulator::hasNextEvent() const
{
    return currentEvent_ < events_.size();
}

SimulationEvent SimpleTestSimulator::nextEvent()
{
    if (!hasNextEvent())
    {
        throw std::out_of_range("No more simulation events available");
    }

    return events_[currentEvent_++];
}

void SimpleTestSimulator::reset()
{
    currentEvent_ = 0;
}