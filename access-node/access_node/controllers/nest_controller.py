import sys
from functools import lru_cache
from more_itertools import sort_together
import connexion
import six
import orjson
from connexion.lifecycle import ConnexionResponse

import datetime
import random
import requests
import numpy as np

from access_node.models.error import Error  # noqa: 
from access_node.models.error_response import ErrorResponse  # noqa: E501
from access_node.models.multimeter_info import MultimeterInfo  # noqa: E501
from access_node.models.multimeter_measurement import MultimeterMeasurement  # noqa: E501
from access_node.models.nest_node_collection_properties import NestNodeCollectionProperties  # noqa: E501
from access_node.models.nest_node_properties import NestNodeProperties  # noqa: E501
from access_node.models.simulation_time_info import SimulationTimeInfo  # noqa: E501
from access_node.models.spikedetector_info import SpikedetectorInfo  # noqa: E501
from access_node.models.spikes import Spikes  # noqa: E501
from access_node import util

from access_node.models.simulation_nodes import simulation_nodes


def nest_get_kernel_status():  # noqa: E501
    """Retreives the current status of the NEST kernel.

     # noqa: E501


    :rtype: List[MultimeterInfo]
    """
    kernel_statuses = [] # Yes, statuses is a valid plural form of status
    for simulation_node in simulation_nodes.nest_simulation_nodes:
        kernel_statuses.append(requests.get(simulation_node+"/kernelStatus").json())
    return kernel_statuses


def nest_get_multimeter_by_id(multimeter_id):  # noqa: E501
    """Retreives the available multimeters and their properties.

     # noqa: E501

    :param multimeter_id: The identifier of the multimeter.
    :type multimeter_id: int

    :rtype: MultimeterInfo
    """
    # Multimeterendpoint doesnt exist in nest module yet
    multimeters = nest_get_multimeters()
    for multimeter in multimeters:
        if multimeter['multimeterId'] is multimeter_id:
            return multimeter
    error = Error(code ="invalidMultimeterID", message = "Multimeter ID doesnt exist.")
    response = ErrorResponse(error)
    return response, 500
    

@lru_cache(maxsize=32)
def nest_get_nodes_by_multimeter_id(multimeter_id):
    mult_info = nest_get_multimeters()
    
    multimeter_exists = False
    for mult in mult_info:
        if mult["multimeterId"] == multimeter_id:
            mult_node_ids = mult['nodeIds']
            return True, mult_node_ids
    if not multimeter_exists:
        error = Error(code ="InvalidMultimeterRequest", message = "Given multimeter does not exist")
        error_response = ErrorResponse(error)
        return False, error_response


def nest_get_multimeter_measurements(multimeter_id, attribute_name, from_time=None, to_time=None, node_ids=None, skip=0, top=0):  # noqa: E501
    """Retrieves the measurements for a multimeter, attribute and node IDs (optional).

     # noqa: E501

    :param multimeter_id: The multimeter to query
    :type multimeter_id: int
    :param attribute_name: The attribute to query (e.g., &#39;V_m&#39; for the membrane potential)
    :type attribute_name: str
    :param from_time: The start time (including) to be queried.
    :type from_time: float
    :param to_time: The end time (excluding) to be queried.
    :type to_time: float
    :param node_ids: A list of node IDs queried for attribute data.
    :type node_ids: List[int]
    :param skip: The offset into the result.
    :type skip: int
    :param top: The maximum number of entries to be returned.
    :type top: int

    :rtype: MultimeterMeasurement
    """
    #TODO Cache this
    mult_found, mult_nodes = nest_get_nodes_by_multimeter_id(multimeter_id)

    node_id_params = node_ids
    if node_ids == None and mult_found:
        node_ids = mult_nodes
    else:
        for node_id in node_ids:
            if node_id not in mult_nodes:
                error = Error(code ="InvalidMultimeterRequest", message = "Node "+str(node_id)+" is not monitored by given Multimeter")
                error_response = ErrorResponse(error)
                return error_response, 400


    init = True
    sim_times = []
    if  simulation_nodes.nest_simulation_nodes == None:
        return

    for node in simulation_nodes.nest_simulation_nodes:

        if node_id_params is not None:
            node_id_param = ",".join(map(str, node_id_params))
        else:
            node_id_param = None

        response = requests.get(
            node+"/multimeter_measurement", params={"multimeterId": multimeter_id, 
            "attribute": attribute_name, "fromTime": from_time,
            "toTime": to_time, "nodeIds": node_id_param})
        response = orjson.loads(response.content)

        if init:
            sim_times = response["simulationTimes"]
            multimeter_values = [None for _ in range(0, (len(sim_times)*len(node_ids)))]
            init = False
        for x in range(len(response['nodeIds'])):
            node_id = response['nodeIds'][x]
            index = node_ids.index(node_id)
            index_offset = index * len(sim_times)
            for y in range(len(sim_times)):
                multimeter_values[index_offset + y] = response['values'][x*len(sim_times)+y]


    # offset and limit
    if skip > 0 or top > 0:
        print("slicing")
        top = len(node_ids) - skip 
        node_ids = node_ids[skip:skip+top]
        multimeter_values = multimeter_values[skip *len(sim_times):(skip+top)*len(sim_times)]

    json_string = orjson.dumps({"simulationTimes":sim_times, "nodeIds":node_ids, "values": multimeter_values})
    return ConnexionResponse(status_code=200, content_type='application/json', mimetype='text/plain',body=json_string)
    # return measurement

@lru_cache(maxsize=32)
def nest_get_multimeters():  # noqa: E501
    """Retreives the available multimeters and their properties.

     # noqa: E501


    :rtype: List[MultimeterInfo]
    """
    multimeters = []
    for simulation_node in simulation_nodes.nest_simulation_nodes:
        multimeters_from_sim_node = requests.get(simulation_node+"/multimeters")
        multimeters_from_sim_node = orjson.loads(multimeters_from_sim_node.content)
        if len(multimeters) == 0:
            multimeters = multimeters_from_sim_node
        else:
            for i in range(len(multimeters)):
                assert(multimeters[i]['multimeterId'] == multimeters_from_sim_node[i]['multimeterId'])
                assert(multimeters[i]['attributes'] == multimeters_from_sim_node[i]['attributes'])
                multimeters[i]['nodeIds'].extend(multimeters_from_sim_node[i]['nodeIds'])

    for multimeter in multimeters:
        multimeter['nodeIds'].sort()

    return multimeters


def nest_get_node_by_id(node_id):  # noqa: E501
    """Retrieves the properties of the specified node.

     # noqa: E501

    :param node_id: The ID of the queried node.
    :type node_id: int

    :rtype: NestNodeProperties
    """
    nodes = nest_get_nodes()
    for node in nodes:
        if node['nodeId'] is node_id:
            return node
    error = Error(code ="invalidNodeID", message = "Node ID doesnt exist.")
    response = ErrorResponse(error)
    return response, 500


def nest_get_node_collections():  # noqa: E501
    """Retrieves a list of all node collection IDs.

     # noqa: E501


    :rtype: List[NestNodeCollectionProperties]
    """
    simulation_node = random.choice(simulation_nodes.nest_simulation_nodes)
    return requests.get(simulation_node+"/nodeCollections").json()
    

def nest_get_node_ids():  # noqa: E501
    """Retrieves a list of all node IDs.

     # noqa: E501


    :rtype: List[int]
    """
    nodes = nest_get_nodes()
    ids = []
    for node in nodes:
        ids.append(node['nodeId'])
    ids.sort()
    return ids


def nest_get_node_ids_by_node_collection(node_collection_id):  # noqa: E501
    """Retrieves the list of all node ids within the node collection.

     # noqa: E501

    :param node_collection_id: The identifier of the node collection
    :type node_collection_id: int

    :rtype: List[int]
    """
    nodes = nest_get_nodes()
    ids = []
    for node in nodes:
        if node['nodeCollectionId'] is node_collection_id:
            ids.append(node['nodeId'])
    if len(ids) is 0:
        error = Error(code ="invalidNodeCollectionID", message = "Node Collection ID doesnt exist.")
        response = ErrorResponse(error)
        return response, 500
    ids.sort()
    return ids


def nest_get_nodes():  # noqa: E501
    """Retrieves all nodes of the simulation.

     # noqa: E501


    :rtype: List[NestNodeProperties]
    """
    nodes = []
    for simulation_node in simulation_nodes.nest_simulation_nodes:
        nodes.extend(requests.get(simulation_node+"/nodes").json())
    return nodes


def nest_get_nodes_by_node_collection(node_collection_id):  # noqa: E501
    """Retrieves the list of all node within the node collection.

     # noqa: E501

    :param node_collection_id: The identifier of the node collection
    :type node_collection_id: int

    :rtype: List[NestNodeProperties]
    """
    all_nodes = nest_get_nodes()
    nodes = []
    for node in all_nodes:
        if node['nodeCollectionId'] is node_collection_id:
            nodes.append(node)
    if len(nodes) is 0:
        error = Error(code ="invalidNodeCollectionID", message = "Node Collection ID doesnt exist.")
        response = ErrorResponse(error)
        return response, 500
    return nodes


def nest_get_simulation_time_info():  # noqa: E501
    """Retrieves simulation time information (stepSize, begin, current, end).

     # noqa: E501


    :rtype: SimulationTimeInfo
    """
    if len(simulation_nodes.nest_simulation_nodes) != 0:
        current_time = float("inf")
        begin = 0
        end = 0
        step_size = 0
        for node in simulation_nodes.nest_simulation_nodes:
            try:
                response = requests.get(node+"/simulationTimeInfo").json()
                current_time = min(current_time, response["current"])
                begin = max(begin, response["begin"])
                end = max(end, response["end"])
                step_size = response["stepSize"]
            except:
                error = Error(code ="SimulationNotRunning", message = "No simulation running at the moment")
                response = ErrorResponse(error)
                return response, 422
        time_info = SimulationTimeInfo(current=current_time, begin=begin, end=end, step_size=step_size)
        return time_info, 200
    else:
        error = Error(code ="SimulationNotRunning", message = "No simulation nodes available")
        response = ErrorResponse(error)
        return response, 500

def nest_get_spikedetectors():
    simulation_node = random.choice(simulation_nodes.nest_simulation_nodes)
    return requests.get(simulation_node+"/spikedetectors").json()

def nest_get_spikerecorders():  # noqa: E501
    """Queries all spike detectors accessable to the pipeline.

     # noqa: E501


    :rtype: List[SpikedetectorInfo]
    """
    simulation_node = random.choice(simulation_nodes.nest_simulation_nodes)
    return requests.get(simulation_node+"/spikerecorders").json()


def nest_get_spikes(from_time=None, to_time=None, node_ids=None, skip=None, top=None):  # noqa: E501
    """Retrieves the spikes for the given time range (optional) and node IDs (optional). If no time range or node list is specified, it will return the spikes for whole time or all nodes respectively. This request merges the spikes recorded by all spike detectors and removes duplicates.

     # noqa: E501

    :param from_time: The start time in milliseconds (including) to be queried.
    :type from_time: float
    :param to_time: The end time in milliseconds (excluding) to be queried.
    :type to_time: float
    :param node_ids: A list of node IDs queried for spike data.
    :type node_ids: List[int]
    :param skip: The offset into the result.
    :type skip: int
    :param top: The maximum number of entries to be returned.
    :type top: int

    :rtype: Spikes  
    """

    app = connexion.FlaskApp(__name__)
    spikes = Spikes([],[])
    simulation_times = []
    node_id_list = []
    lastFrame =  False

    for node in simulation_nodes.nest_simulation_nodes:
        if node_ids is not None:
            node_id_param = ",".join(map(str, node_ids))        
        else:
            node_id_param = None        

        response = requests.get(
            node+"/spikes", params={"fromTime": from_time, "toTime": to_time, "nodeIds": node_id_param})
        response = orjson.loads(response.content)
        simulation_times = simulation_times + response["simulationTimes"]
        node_id_list = node_id_list + response["nodeIds"]
        lastFrame = response["lastFrame"]


    #sort
    sorted_lists = sort_together([simulation_times,node_id_list])
    if sorted_lists != []:
        spikes.simulation_times = sorted_lists[0]
        spikes.node_ids = sorted_lists[1]

    # offset and limit
    if (skip is None):
        skip = 0
    if (top is None or (top + skip) > len(spikes.node_ids)):
        top = len(spikes.node_ids) - skip
    spikes.node_ids = spikes.node_ids[skip:skip+top]
    spikes.simulation_times = spikes.simulation_times[skip:skip+top]

    json_string = orjson.dumps({"nodeIds":spikes.node_ids,"simulationTimes":spikes.simulation_times,"lastFrame":lastFrame})
    return ConnexionResponse(status_code=200,content_type='application/json', mimetype='text/plain', body=json_string)


def nest_get_spikes_by_node_collection(node_collection_id, from_time=None, to_time=None, skip=None, top=None):  # noqa: E501
    """Retrieves the spikes for the given simulation steps (optional) and node collection. This request merges the spikes recorded by all spike detectors and removes duplicates.

     # noqa: E501

    :param node_collection_id: The identifier of the node collection.
    :type node_collection_id: int
    :param from_time: The start time (including) to be queried.
    :type from_time: float
    :param to_time: The end time (excluding) to be queried.
    :type to_time: float
    :param skip: The offset into the result.
    :type skip: int
    :param top: The maximum numbers of entries to be returned.
    :type top: int

    :rtype: Spikes
    """
    spikes = Spikes([], [])
    simulation_times = []
    node_id_list = []
    lastFrame=False
    for node in simulation_nodes.nest_simulation_nodes:
        response = requests.get(
            node+"/spikes", params={"fromTime": from_time, "toTime": to_time, "nodeCollectionId": node_collection_id})
        response = orjson.loads(response.content)
        lastFrame = response["lastFrame"]
        simulation_times = simulation_times + response["simulationTimes"]
        node_id_list = node_id_list + response["nodeIds"]


    # sort
    sorted_lists = sort_together([simulation_times,node_id_list])
    if sorted_lists != []:
        spikes.simulation_times = sorted_lists[0]
        spikes.node_ids = sorted_lists[1]

    # offset and limit
    if (skip is None):
        skip = 0
    if (top is None or (top + skip) > len(spikes.node_ids)):
        top = len(spikes.node_ids) - skip
    spikes.node_ids = spikes.node_ids[skip:skip+top]
    spikes.simulation_times = spikes.simulation_times[skip:skip+top]

    json_string = orjson.dumps({"nodeIds":spikes.node_ids,"simulationTimes":spikes.simulation_times,"lastFrame":lastFrame})
    return ConnexionResponse(status_code=200,content_type='application/json', mimetype='text/plain', body=json_string)

def nest_get_spikes_by_spikedetector(spikedetector_id, from_time=None, to_time=None, node_ids=None, skip=None, top=None):
    return nest_get_spikes_by_spikerecorder(spikedetector_id, from_time, to_time, node_ids, skip, top)

def nest_get_spikes_by_spikerecorder(spikerecorder_id, from_time=None, to_time=None, node_ids=None, skip=None, top=None):  # noqa: E501
    """Retrieves the spikes for the given time range (optional) and node IDs (optional) from one spike detector. If no time range or node list is specified, it will return the spikes for whole time or all nodes respectively.

     # noqa: E501

    :param spikedetector_id: The ID of the spike detector to query.
    :type spikedetector_id: int
    :param from_time: The start time in milliseconds (including) to be queried.
    :type from_time: float
    :param to_time: The end time in milliseconds (excluding) to be queried.
    :type to_time: float
    :param node_ids: A list of node IDs queried for spike data.
    :type node_ids: List[int]
    :param skip: The offset into the result.
    :type skip: int
    :param top: The maximum number of entries to be returned.
    :type top: int

    :rtype: Spikes
    """
    spikes = Spikes([], [])
    simulation_times = []
    node_id_list = []
    lastFrame = False

    for node in simulation_nodes.nest_simulation_nodes:
        
        if node_ids is not None:
            node_id_param = ",".join(map(str, node_ids))
        else:
            node_id_param = None

        response = requests.get(node+"/spikes", params={"fromTime": from_time, "toTime": to_time, "nodeIds": node_id_param, "spikedetectorId": spikerecorder_id})
         
        response = orjson.loads(response.content)
        lastFrame = response["lastFrame"]
        simulation_times = simulation_times + response["simulationTimes"]
        node_id_list = node_id_list + response["nodeIds"]

    #sort
    sorted_lists = sort_together([simulation_times,node_id_list])
    if sorted_lists != []:
        spikes.simulation_times = sorted_lists[0]
        spikes.node_ids = sorted_lists[1]



    # offset and limit
    if (skip is None):
        skip = 0
    if (top is None or (top + skip) > len(spikes.node_ids)):
        top = len(spikes.node_ids) - skip
    spikes.node_ids = spikes.node_ids[skip:skip+top]
    spikes.simulation_times = spikes.simulation_times[skip:skip+top]

    json_string = orjson.dumps({"nodeIds":spikes.node_ids,"simulationTimes":spikes.simulation_times, "lastFrame":lastFrame})
    return ConnexionResponse(status_code=200,content_type='application/json', mimetype='text/plain', body=json_string)

