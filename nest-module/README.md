# Insite Nest Module

## Usage
### Docker
1. Install [Docker](https://www.docker.com).
2. Build the docker container `docker build -t insite-nest-module .`. This will take a while on the first run.
3. Run the example simulation `docker run --publish 9000:9000 insite-nest-module <time>` where `<time>` specifies the amount of biological time in milliseconds that should be simulated.
4. Access the spikes at `http://localhost:9000/spikes`.
