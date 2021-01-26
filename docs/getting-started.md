# Getting Started

## Installation
1. For easy deployment of the pipeline we use docker containers, so make sure you have the [docker engine](https://www.docker.com) installed.
2. Clone this repository (`git clone https://github.com/VRGroupRWTH/insite.git`).
3. Build the containers by using the command `docker-compose build` in the root directory of the repository. On the first run, this can take quite some time. Subsequent runs will usually run much faster as docker caches intermediate builds.

## Verify Installation
1. Start the the test simulation by using the command `docker-compose up` in the root directory of the repository. To make sure the latest changes are included it is advised to use `docker-compose up --build` to avoid errors by out-of-date containers.
2. You should now be able to query `http://localhost:8080/version` which should give an output similar to this:
   ```json
   {
       "api": "1.0",
       "insite": "1.0"
   }
   ```
   Where `insite` specifies the version of the pipeline and `api` the available endpoint versions for the REST API.
3. You can query simulation specific queries using the `https://localhost:8080/nest/*` endpoints. E.g., `https://localhost:8080/nest/nodes` to get all nest nodes. The complete API is available [here](docs/api/README.md).
