# Insite
This is the main repository for the in-situ-pipeline developed in Task 7.3.8 for the HBP.

## Testing
1. For easy testing and deployment we use docker containers, so make sure you have the [docker engine](https://www.docker.com) installed.
2. Clone the repository: `git clone https://devhub.vr.rwth-aachen.de/VR-Group/in-situ-pipeline/insite.git`
3. Update the submodules: `git submodule update --init`
4. Run `docker-compose up --build` in the root directory.
5. After the docker containers are built and started, the REST API for a demo simulation is available at `http://localhost:8079`.

## API Reference
The reference of the REST API can be found [here](https://devhub.vr.rwth-aachen.de/VR-Group/in-situ-pipeline/insite/wikis/api/api_reference).