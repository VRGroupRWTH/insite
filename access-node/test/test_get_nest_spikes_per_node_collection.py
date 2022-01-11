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
    model = "model"

#Tests a NEST_GET_SPIKES_PER_NODE_NODE_COLLECTION_REQUEST by checking if every returned collection can be accessed via its own URL
def test_get_nest_spikes_per_node_collection(nest_simulation):
    node_collections = return_json_body_if_status_ok(URL_NEST_GET_SPIKES_PER_NODE_COLLECTION)
  
    assert(len(node_collections ) > 0)

    collection_ids = []

    for collection in node_collections:
      collection_ids.append(collection[NEST_GET_SPIKES_PER_NODE_COLLECTION_PROPERTY_NAMES.nodeCollectionId])
      assert(NEST_GET_SPIKES_PER_NODE_COLLECTION_PROPERTY_NAMES.nodes in collection)
      assert(NEST_GET_SPIKES_PER_NODE_COLLECTION_PROPERTY_NAMES.nodeCollectionId in collection)
      assert(NEST_GET_SPIKES_PER_NODE_COLLECTION_PROPERTY_NAMES.model in collection)
      assert(len(collection[NEST_GET_SPIKES_PER_NODE_COLLECTION_PROPERTY_NAMES.nodes]) > 0)
      assert(len(collection[NEST_GET_SPIKES_PER_NODE_COLLECTION_PROPERTY_NAMES.model]) > 0)
    
    assert(len(collection_ids) > 0)

    for collection_id in collection_ids:
      url = URL_NEST_GET_SPIKES_PER_NODE_COLLECTION + "/{0}/spikes".format(collection_id)
      spikes = return_json_body_if_status_ok(url)

      current_collection = next(item for item in node_collections if item[NEST_GET_SPIKES_PER_NODE_COLLECTION_PROPERTY_NAMES.nodeCollectionId] == collection_id)

      for node_id in spikes[NEST_GET_SPIKES_PER_NODE_COLLECTION_PROPERTY_NAMES.nodeIds]:
        assert(node_id <= current_collection[NEST_GET_SPIKES_PER_NODE_COLLECTION_PROPERTY_NAMES.nodes][NEST_GET_SPIKES_PER_NODE_COLLECTION_PROPERTY_NAMES.lastId])
        assert(node_id >= current_collection[NEST_GET_SPIKES_PER_NODE_COLLECTION_PROPERTY_NAMES.nodes][NEST_GET_SPIKES_PER_NODE_COLLECTION_PROPERTY_NAMES.firstId])

     
  