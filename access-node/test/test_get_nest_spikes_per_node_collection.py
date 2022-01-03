import time
import numbers
import math
import itertools
from operator import truediv
import requests
from requests.api import request
from requests.sessions import Request
from nest_general_test_functions import *

#URL used for the "nest_get_spikes_per_node_collection" HTTP-query
URL_NEST_GET_SPIKES_PER_NODE_COLLECTION = BASE_REQUEST_URL + "/nodeCollections"

#Names for the "nest_get_spikes_per_node_collection" property names
class NEST_GET_SPIKES_PER_NODE_COLLECTION_PROPERTY_NAMES:
    nodeCollectionId = "nodeCollectionId"
    toTime = "toTime"
    nodeIds = "nodeIds"
    nodes = "nodes"
    lastId = "lastId"
    firstId = "firstId"

#Tests a NEST_GET_SPIKES_PER_NODE_NODE_COLLECTION_REQUEST by checking if every returned collection can be accessed via its own URL
def test_get_nest_spikes_per_node_collection(nest_simulation):
    node_collections = check_request_valid(URL_NEST_GET_SPIKES_PER_NODE_COLLECTION)

    collection_ids = []

    for collection in node_collections:
      collection_ids.append(collection[NEST_GET_SPIKES_PER_NODE_COLLECTION_PROPERTY_NAMES.nodeCollectionId])
    
    for collection_id in collection_ids:
      url = URL_NEST_GET_SPIKES_PER_NODE_COLLECTION + "/{0}/spikes".format(collection_id)
      spikes = check_request_valid(url)

      current_collection = next(item for item in node_collections if item[NEST_GET_SPIKES_PER_NODE_COLLECTION_PROPERTY_NAMES.nodeCollectionId] == collection_id)

      for node_id in spikes[NEST_GET_SPIKES_PER_NODE_COLLECTION_PROPERTY_NAMES.nodeIds]:
        assert(node_id <= current_collection[NEST_GET_SPIKES_PER_NODE_COLLECTION_PROPERTY_NAMES.nodes][NEST_GET_SPIKES_PER_NODE_COLLECTION_PROPERTY_NAMES.lastId])
        assert(node_id >= current_collection[NEST_GET_SPIKES_PER_NODE_COLLECTION_PROPERTY_NAMES.nodes][NEST_GET_SPIKES_PER_NODE_COLLECTION_PROPERTY_NAMES.firstId])

     
  