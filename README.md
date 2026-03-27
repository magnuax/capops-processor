# capops-processor

A real-time **Capacity Operations** monitoring system for air traffic control. It processes flight tracking data and weather information to monitor airspace capacity across geographic sectors, detect risk events, and publish live data to API-backend via Redis.

## Features

- **Grid-based sector monitoring** — divides airspace into configurable lat/lon cells and tracks per-sector aircraft counts and capacity
- **Dual data sources** — run with simulated flights and weather, or connect to live APIs (OpenSky Network for flights, Open-Meteo for weather)
- **Weather-adjusted capacity** — sector capacity is dynamically reduced based on weather severity (OK → DEGRADED → SEVERE → EXTREME)
- **Risk event detection** — flags sectors as CONGESTED or AT_RISK when aircraft counts exceed effective capacity
- **Real-time publishing** — serializes all data to Protocol Buffers and publishes to a Redis channel for downstream consumers

## Architecture

```
Track Source ──→ IngestService ──→ ComputeData ──→ ProtoMapper ──→ RedisPublisher
                                       ↑
Weather Source ─→ IngestService ───────┘
```

Each iteration of the main loop:

1. Tick data sources (simulators advance state, APIs fetch latest data)
2. Ingest tracks and weather into the compute grid
3. Evaluate sector states and generate risk events
4. Serialize the result to Protobuf and publish to Redis

## Prerequisites

| Tool | Version |
|------|---------|
| CMake | ≥ 3.16 |
| C++ compiler | C++17 support |
| Ninja | recommended |
| Qt6 | ≥ 6.8.3 (Core, Network) |
| Protobuf | 3.x (compiler + libraries) |

The following are fetched automatically via CMake `FetchContent`:

- [hiredis](https://github.com/redis/hiredis) 1.3.0
- [redis-plus-plus](https://github.com/sewenew/redis-plus-plus) 1.3.9
- [Catch2](https://github.com/catchorg/Catch2) 3.5.4 (tests only)

## Building

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## Running

```bash
./build/backend
```

The processor reads `configuration.cfg` from the working directory on startup.

## Testing

```bash
cmake --build build
ctest --test-dir build --output-on-failure
```

Tests use Catch2 and cover configuration loading, grid computation, domain entities, the data processing pipeline, proto mapping, simulators, risk event detection, and Redis publishing.

## Docker

```bash
docker build -t capops-processor .
docker run --rm --network capops-network capops-processor
```

The image is based on Ubuntu 24.04 and performs a full in-container build. The CI/CD pipeline automatically pushes `jacobg431/processor:latest` to DockerHub on commits to `main`.

## Configuration

`configuration.cfg` uses an INI-style format:

```ini
[grid]
minLat=59.0
maxLat=61.0
minLon=4.0
maxLon=6.0
cellSizeDeg=0.1
coordinateSystem=WGS84

[capacity]
defaultBaseCapacity=1

[weatherFactors]
OK=1.0
DEGRADED=0.8
SEVERE=0.6
EXTREME=0.4

[redis]
redisUrl=tcp://redis-server:6379
redisChannel=flightdata

[dataSource]
type=sim          # "sim" for simulation, "api" for live data

[simulation]
numFlights=3
timestepSize=1

[execution]
loopIntervalMs=1000
```

## Project Structure

```
├── main.cpp                  # Entry point
├── configuration.cfg         # Runtime configuration
├── proto/
│   └── FlightData.proto      # Protobuf message definitions
├── include/
│   ├── app/                  # Application orchestrator
│   ├── config/               # Configuration management
│   ├── domain/               # Core entities (Track, Sector, RiskEvent, …)
│   ├── sources/              # Data source interfaces and implementations
│   │   ├── interfaces/       #   ITrackSource, IWeatherSource
│   │   └── simulations/      #   RadarSimulator, WeatherSimulator
│   ├── compute/              # Grid and data processing
│   ├── ingest/               # Data ingestion service
│   ├── publish/              # Redis publishing and proto mapping
│   └── utils/                # Timestamp utilities
├── src/                      # Corresponding implementations
└── tests/
    └── test.cpp              # Catch2 unit tests
```

## License

This project is licensed under the [GNU General Public License v3.0](LICENSE).
