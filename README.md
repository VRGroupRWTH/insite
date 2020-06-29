# Insite
This is the main repository for the in-situ-pipeline developed in Task 5.7 for the HBP.

## Installation
1. For easy deployment of the pipeline we use docker containers, so make sure you have the [docker engine](https://www.docker.com) installed.
2. Clone this repository (`git clone https://github.com/VRGroupRWTH/insite.git`) or download the `docker-compose.yml` file to your computer.

## Verify Installation
1. To verify that everything works, run `docker-compose run insite-nest /example/brunel_simulation.py` from the directory containing the `docker-compose.yml` file.
2. On the first launch, docker will download all required images. This may take some time. Subsequent runs will finish considerably faster.
3. After the required images are downloaded a simple NEST simulation is run. You should be able to type in `http://localhost:8080/nest/spikes` into your web browser and see an overview of all neurons in the simulation. You can also try out other queries defined in the [API](https://devhub.vr.rwth-aachen.de/VR-Group/in-situ-pipeline/insite/wikis/api/api_reference).

## Run a Custom NEST Simulation
1. Load and connect the insite nest module. You can have a look at [this example](https://github.com/VRGroupRWTH/insite-nest-module/blob/master/example/brunel_simulation.py) to see how the code could look like.
    1. Add `nest.Install("insitemodule")` at the beginning of the simulation to load the nest module.
    2. Create a spike detector `spike_detector = nest.Create("spike_detector")` and set it up to record its data to insite: `nest.SetStatus(spike_detector, [{"record_to": "insite"}])`.
    3. Connect the spike detector to all neurons you want to be able to observe at run-time `nest.Connect(neurons, spike_detector)`.
2. To run your custom simulation you have to add it to the cocker container. This can be done using the following command: `docker-compose run -v PATH_TO_SIMULATION:INTERNAL_PATH:ro insite-nest INTERNAL_PATH/ENTRY_POINT [NUM_MPI_PROCESSES]`.
   * `PATH_TO_SIMULATION` should be the directory containing all files of your simulation. **IMPORTANT: the path must be absolute.**
   * `INTERNAL_PATH` points to some path inside the container and does not matter too much. Normally you can just set it to `/sim`.
   * `ENTRY_POINT` should specify the main python file of the simulation (relative to the directory specified by `PATH_TO_SIMULATION`).
   * Finally, you can enable MPI by specifying the number of processes it should use with `NUM_MPI_PROCESSES`. If you do not specify this parameter, MPI will be disabled.

    The complete call to run the simulation in the example directory would look like this: `docker-compose run -v $(pwd)/example/:/sim:ro insite-nest /sim/brunel_simulation.py`.
