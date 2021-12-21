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
URL_NEST_GET_NODE_COLECTIONS = BASE_REQUEST_URL + "/nodeCollections"

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
    node_collections = check_request_valid(URL_NEST_GET_NODE_COLECTIONS)

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