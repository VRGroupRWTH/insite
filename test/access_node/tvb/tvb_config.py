from enum import Enum

#Base URL and prefix for every HTTP-querie to the Arbor-Server
BASE_REQUEST_URL = "http://insite-access-node:52056/tvb"

#Relates every value to its corresponding name in the spike-data JSON-field
class JSON_VALUE_TO_FIELD_NAME(Enum):
    simulationTimes = "simulationTimes"
    nodeIds = "gIds"
    spikerecorderId = "spikerecorderId"
    spikerecorders = "spikerecorders"
    spikes = "spikes"
    lastFrame = "lastFrame"
    simulationId = "simId"
    nodeCollections = "nodeCollections"
    kernelStatuses = "kernelStatuses"
