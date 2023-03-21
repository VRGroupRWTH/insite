import time
import numbers
import math
import itertools
from operator import truediv
import requests
from requests.api import request
from requests.sessions import Request
from nest_general_test_functions import *

#URL used for the NEST_GET_NODE_COLLECTIONS HTTP-request
URL_NEST_GET_NODE_COLLECTIONS = BASE_REQUEST_URL + "/nodeCollections"
URL_NEST_GET_NODE_COLLECTIONS_V2 = BASE_REQUEST_URL_V2 + "/nodeCollections"

#Dictionary that matches the names included under the NODES-entry in the GET_NODE_COLLECTIONS result to their corresponding data-types
NODES_JSON_LIST_ENTRIES_WIRH_DATA_TYPES = {
  "count" : int,
  "firstId" : int,
  "lastId" : int
}

#Dictionary that matches the names included under the MODEL-entry in the GET_NODE_COLLECTIONS result  to their corresponding data-types
MODEL_JSON_LIST_ENTRIES_WIRH_DATA_TYPES = {
  "name" :  str,
  "status" : dict
}

#Dictionary that matches the names of the GET_NODE_COLLECTIONS result  to their corresponding data-types
NODE_COLLECTION_JSON_LIST_ENTRIES_WIRH_DATA_TYPES = {  
  "model" : MODEL_JSON_LIST_ENTRIES_WIRH_DATA_TYPES,
  "nodeCollectionId" : int,
  "nodes" : NODES_JSON_LIST_ENTRIES_WIRH_DATA_TYPES
}

#Tests the NEST_GET_NODE_COLLECTIONS by checking if the response code is valid, all required information is included and the has the correct data-type for every collection
def test_nest_get_node_collections(nest_simulation):
    node_collections = return_json_body_if_status_ok(URL_NEST_GET_NODE_COLLECTIONS)

    for collection in node_collections:
      assert(isinstance(collection, dict))
      
      for collection_entry, colection_entry_data_type in NODE_COLLECTION_JSON_LIST_ENTRIES_WIRH_DATA_TYPES.items():
        assert(collection_entry in collection)

        if isinstance(colection_entry_data_type, dict):

          for sub_entry, sub_data_type in NODE_COLLECTION_JSON_LIST_ENTRIES_WIRH_DATA_TYPES[collection_entry].items():
            assert(sub_entry in collection[collection_entry])
            assert(isinstance(collection[collection_entry][sub_entry], sub_data_type))

            if sub_data_type == dict:
              assert(len(collection[collection_entry][sub_entry]) > 0)
        else:
          assert(isinstance(collection[collection_entry], colection_entry_data_type))


#Tests the NEST_GET_NODE_COLLECTIONS by checking if the response code is valid, all required information is included and the has the correct data-type for every collection
def test_nest_get_node_collections_v2(nest_simulation):
    json_response = return_json_body_if_status_ok(URL_NEST_GET_NODE_COLLECTIONS_V2)
    assert(JSON_VALUE_TO_FIELD_NAME.simulationId.value in json_response)

    node_collections = json_response[JSON_VALUE_TO_FIELD_NAME.nodeCollections.value]

    for collection in node_collections:
      assert(isinstance(collection, dict))
      
      for collection_entry, colection_entry_data_type in NODE_COLLECTION_JSON_LIST_ENTRIES_WIRH_DATA_TYPES.items():
        assert(collection_entry in collection)

        if isinstance(colection_entry_data_type, dict):

          for sub_entry, sub_data_type in NODE_COLLECTION_JSON_LIST_ENTRIES_WIRH_DATA_TYPES[collection_entry].items():
            assert(sub_entry in collection[collection_entry])
            assert(isinstance(collection[collection_entry][sub_entry], sub_data_type))

            if sub_data_type == dict:
              assert(len(collection[collection_entry][sub_entry]) > 0)
        else:
          assert(isinstance(collection[collection_entry], colection_entry_data_type))
