from enum import Enum

#Base URL and prefix for every HTTP-querie to the NEST-Server
BASE_REQUEST_URL = "http://localhost:52056/nest"
BASE_REQUEST_URL_V2 = "http://localhost:52056/v2/nest"

#Relates every value to its corresponding name in the spike-data JSON-field
class JSON_VALUE_TO_FIELD_NAME(Enum):
    simulationTimes = "simulationTimes"
    nodeIds = "nodeIds"
    spikerecorderId = "spikerecorderId"
    spikerecorders = "spikerecorders"
    spikes = "spikes"
    lastFrame = "lastFrame"
    simulationId = "simId"
    nodeCollections = "nodeCollections"
    kernelStatuses = "kernelStatuses"