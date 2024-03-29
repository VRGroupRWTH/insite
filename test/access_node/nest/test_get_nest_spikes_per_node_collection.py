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
URL_NEST_GET_SPIKES_PER_NODE_COLLECTION_V2 = BASE_REQUEST_URL_V2 + "/nodeCollections"

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
      url = (URL_NEST_GET_SPIKES_PER_NODE_COLLECTION + "/{0}/spikes").format(collection_id)
      spikes = return_json_body_if_status_ok(url)
      
      spikes_is_data_length_valid(spikes, True)
      # spikes_is_sorted_by_time(spikes)
      spikes_nodeIds_are_greater_or_equal_than(spikes, 0)
      spikes_simulation_times_are_greater_or_equal_than(spikes, 0)

      current_collection = next(item for item in node_collections if item[NEST_GET_SPIKES_PER_NODE_COLLECTION_PROPERTY_NAMES.nodeCollectionId] == collection_id)

      for node_id in spikes[NEST_GET_SPIKES_PER_NODE_COLLECTION_PROPERTY_NAMES.nodeIds]:
        assert(node_id <= current_collection[NEST_GET_SPIKES_PER_NODE_COLLECTION_PROPERTY_NAMES.nodes][NEST_GET_SPIKES_PER_NODE_COLLECTION_PROPERTY_NAMES.lastId])
        assert(node_id >= current_collection[NEST_GET_SPIKES_PER_NODE_COLLECTION_PROPERTY_NAMES.nodes][NEST_GET_SPIKES_PER_NODE_COLLECTION_PROPERTY_NAMES.firstId])
        
def test_get_nest_spikes_per_node_collection_v2(nest_simulation):
    json_response = return_json_body_if_status_ok(URL_NEST_GET_SPIKES_PER_NODE_COLLECTION_V2)
    assert(JSON_VALUE_TO_FIELD_NAME.simulationId.value in json_response)

    node_collections = json_response[JSON_VALUE_TO_FIELD_NAME.nodeCollections.value]
  
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
      url = (URL_NEST_GET_SPIKES_PER_NODE_COLLECTION_V2 + "/{0}/spikes").format(collection_id)
      spikes = return_json_body_if_status_ok(url)

      assert(JSON_VALUE_TO_FIELD_NAME.simulationId.value in spikes)
      
      spikes_is_data_length_valid(spikes, True)
      # spikes_is_sorted_by_time(spikes)
      spikes_nodeIds_are_greater_or_equal_than(spikes, 0)
      spikes_simulation_times_are_greater_or_equal_than(spikes, 0)

      current_collection = next(item for item in node_collections if item[NEST_GET_SPIKES_PER_NODE_COLLECTION_PROPERTY_NAMES.nodeCollectionId] == collection_id)

      for node_id in spikes[NEST_GET_SPIKES_PER_NODE_COLLECTION_PROPERTY_NAMES.nodeIds]:
        assert(node_id <= current_collection[NEST_GET_SPIKES_PER_NODE_COLLECTION_PROPERTY_NAMES.nodes][NEST_GET_SPIKES_PER_NODE_COLLECTION_PROPERTY_NAMES.lastId])
        assert(node_id >= current_collection[NEST_GET_SPIKES_PER_NODE_COLLECTION_PROPERTY_NAMES.nodes][NEST_GET_SPIKES_PER_NODE_COLLECTION_PROPERTY_NAMES.firstId])
        