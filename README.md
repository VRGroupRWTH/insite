# Insite
This is the main repository for the in-situ-pipeline developed in Task 5.7 for the HBP.
The in-situ-pipeline enables the user to get simulation data while the simulation is still running.
In the current version, the user can query the HTTP endpoint provided by the pipeline to get simular data encoded as JSON.
To use the in-situ-pipeline the user has to activate the in-situ plugin for the neuronal simulator and deploy the pipeline's access node.
Currently the [NEST simulator](https://www.nest-simulator.org/) is supported by the in-situ-pipeline.

<br>

# Documentation
You can find the information provided in this readme and the complete API [here](https://vrgrouprwth.github.io/insite/)

<br>

# Installation
To provide an easy to use environment the in-situ-pipeline can be deployed by using docker.
To be able to use the in-situ-pipeline please make sure that the [docker engine](https://www.docker.com) and [docker compose](https://docs.docker.com/compose/) are installed. 

Two docker image are provided for the in-situ-pipeline. 
The first image contains the NEST simulator with the in-situ plugin installed.
The second image contains the in-situ-pipeline's access-node.


Generally, there are two different approaches to use the in-situ pipeline with docker:

1. [Using docker-hub](#Using-docker-hub)

2. [Building from GIT](#Building-from-GIT)
___
<br>

## Using docker-hub
The easiest way to set up the in-situ-pipeline is by using the images from docker-hub.
The images can simply be pulled from docker-hub and can be used as is.
The following docker-compose file can be used:
```yml
version: '3'

services:
  access-node:
    image: rwthvr/insite-access-node
    ports:
      - "8080:8080"
    depends_on:
      - "insite-nest-module"
    command: ["http://insite-nest-module:9000"]

  insite-nest-module:
    image: rwthvr/insite-nest-module
    ports:
      - "8000-8099:9000-9099"
    volumes:
      - ./scripts:/scripts/
```
Rename the file to `docker-compose.yml` and use the command `docker-compose up` to deploy the in-situ pipeline. Afterwards, you can [verify the installation](#Verify-Installation) or take a look at [configuiring the in-situ pipeline](#Configure-the-in-situ-pipeline).

<br>
In case that you want to use the latest develop branch instead of the stable master branch, replace 

```yml
access-node:
    image: rwthvr/insite-access-node
    ...

insite-nest-module:
    image: rwthvr/insite-nest-module
    ...

``` 
with 
```yml
access-node:
    image: rwthvr/insite-access-node:develop
    ...

insite-nest-module:
    image: rwthvr/insite-nest-module:develop
    ...

``` 
<br><br>

## Building from GIT
1. Clone this repository (`git clone https://github.com/VRGroupRWTH/insite.git`).
2. Build the containers by using the command `docker-compose build` in the root directory of the repository. On the first run, this can take quite some time as NEST and the NEST plugin must be built. Subsequent runs will usually run much faster as docker caches intermediate builds.

<br>

# Verify Installation
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

<br>

# Configuration

## Pipeline Ports
If you want to expose the REST API on a different port change the following line in your docker-compose file:
```yml
access-node:
    ...
    ports:
      - "XXXX:8080"
    ...
``` 
where `XXXX` is your new port number, afterwards the REST API will be rechable via `IP:XXXX`, e.g. `http://localhost:XXXX`.

If you want to change the exposed ports of the simulations API change the following line in the docker-compose file:
```yml
insite-nest-module:
    ...
    ports:
      - "XXXX-YYYY:9000-9099"
    ...
``` 
Please note that the port range specified on the left must have the same size as the port range specified on the right side. The number of ports which are used will depend on the number of MPI ranks and therefore number of simulation nodes you use. (See [Number of simulation nodes](#Number-of-simulation-nodes))

## Simulation Script

The in-situ-pipeline will come with an example simulation. However, you can also use your own NEST simulation script.
You can place custom scripts in the `script` folder and all `.py` will be picked up automically.
If you have more than one python script in the `script` folder you can specify which script will be used by settings the environment variable `SCRIPT_NAME` by adding the appropriate env variable to the docker-compose file, e.g.:
```yml
insite-nest-module:
    image: rwthvr/insite-nest-module:develop
    environment:
      - SCRIPT_NAME=brunel.py
    ...
```
You can pass additional arguments to your script by setting the environment var `SCRIPT_ARGS`. Individual arguments must be seperated by a comma, e.g.:
```yml
insite-nest-module:
    image: rwthvr/insite-nest-module:develop
    environment:
      - SCRIPT_NAME=brunel.py
      - SCRIPT_ARGS=var1;var2;var3
    ...
```

You can also specify a different script folder by changing the following line in the docker-compose file:
```yml
volume:
      - XXX:/scripts/
```
where `XXX` is the path to the folder.

## Number of simulation nodes

The number of simulation nodes can be adjusted by settings the environment variable `MPI_RANKS`.(Default=1) <br>
E.g., if the NEST simulation should run with 3 simulation nodes:
```yml
insite-nest-module:
    image: rwthvr/insite-nest-module:develop
    environment:
      - MPI_RANKS=3
    ...
```

Additionally, the address of the NEST-module must be added to the access-node's command array. E.g:
```
command: ["http://insite-nest-module:9000", "http://insite-nest-module:9001"]
```
When using the docker images "insite-nest-module" can be used as the hostname. The port port is always structured as 90XY where XY is MPI rank. For rank 3 this would be 9003 for example.
