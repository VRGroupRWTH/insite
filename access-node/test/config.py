from enum import Enum

#Base URL and prefix for every HTTP-querie to the NEST-Server
BASE_REQUEST_URL = "http://localhost:8080/nest"

#Relates every value to its corresponding name in the spike-data JSON-field
class JSON_VALUE_TO_FIELD_NAME(Enum):
    simulationTimes = "simulationTimes"
    nodeIds = "nodeIds"
    spikedetectorId = "spikedetectorId"
    spikes = "spikes"
    lastFrame = "lastFrame"