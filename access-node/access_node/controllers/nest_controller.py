import connexion
import six

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
    simulation_node = random.choice(simulation_nodes.nest_simulation_nodes)
    return requests.get(simulation_node+"/kernelStatus").json()


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
    

def nest_get_multimeter_measurements(multimeter_id, attribute_name, from_time=None, to_time=None, node_ids=None, skip=None, top=None):  # noqa: E501
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
    mult_info = nest_get_multimeters()

    mult_node_ids = []
    multimeter_exists = False
    for mult in mult_info:
        if mult["multimeterId"] == multimeter_id:
            multimeter_exists = True
            if attribute_name not in mult['attributes']:
                error = Error(code ="InvalidMultimeterRequest", message = "Given multimeter does not measure given attribute")
                error_response = ErrorResponse(error)
                return error_response, 400
            mult_node_ids = mult['nodeIds']
            break
    if not multimeter_exists:
        error = Error(code ="InvalidMultimeterRequest", message = "Given multimeter does not exist")
        error_response = ErrorResponse(error)
        return error_response, 400

    if node_ids == None:
        node_ids = mult_node_ids
    else:
        for node_id in node_ids:
            if node_id not in mult_node_ids:
                error = Error(code ="InvalidMultimeterRequest", message = "Node "+str(node_id)+" is not monitored by given Multimeter")
                error_response = ErrorResponse(error)
                return error_response, 400

    init = True
    sim_times = []
    measurement = MultimeterMeasurement([], [], [])
    for node in simulation_nodes.nest_simulation_nodes:
        response = requests.get(
            node+"/multimeter_measurement", params={"multimeterId": multimeter_id, 
            "attribute": attribute_name, "fromTime": from_time,
            "toTime": to_time, "nodeIds": node_ids}).json()
        if init:
            sim_times = response["simulationTimes"]
            measurement = MultimeterMeasurement(
                sim_times, node_ids, [None for x in range(0, (len(sim_times)*len(node_ids)))])
            init = False
        for x in range(len(response['nodeIds'])):
            node_id = response['nodeIds'][x]
            index = measurement.node_ids.index(node_id)
            index_offset = index * len(sim_times)
            for y in range(len(sim_times)):
                measurement.values[index_offset + y] = response['values'][x*len(sim_times)+y]

    # offset and limit
    if (skip is None):
        skip = 0
    if (top is None or (top + skip) > len(measurement.node_ids)):
        top = len(measurement.node_ids) - skip
    measurement.node_ids = measurement.node_ids[skip:skip+top]
    measurement.values = measurement.values[skip *
                                            len(sim_times):(skip+top)*len(sim_times)]

    return measurement


def nest_get_multimeters():  # noqa: E501
    """Retreives the available multimeters and their properties.

     # noqa: E501


    :rtype: List[MultimeterInfo]
    """
    simulation_node = random.choice(simulation_nodes.nest_simulation_nodes)
    return requests.get(simulation_node+"/multimeters").json()


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
            response = requests.get(node+"/simulationTimeInfo").json()
            current_time = min(current_time, response["current"])
            begin = max(begin, response["begin"])
            end = max(end, response["end"])
            step_size = response["stepSize"]

        time_info = SimulationTimeInfo(current=current_time, begin=begin, end=end, step_size=step_size)
        return time_info, 200
    else:
        error = Error(code ="SimulationNotRunning", message = "No simulation nodes available")
        response = ErrorResponse(error)
        return response, 500


def nest_get_spikedetectors():  # noqa: E501
    """Queries all spike detectors accessable to the pipeline.

     # noqa: E501


    :rtype: List[SpikedetectorInfo]
    """
    simulation_node = random.choice(simulation_nodes.nest_simulation_nodes)
    return requests.get(simulation_node+"/spikedetectors").json()


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
    spikes = Spikes([], [])
    for node in simulation_nodes.nest_simulation_nodes:
        response = requests.get(
            node+"/spikes", params={"fromTime": from_time, "toTime": to_time, "nodeIds": node_ids}).json()
        for x in range(len(response["simulationTimes"])):
            if node_ids is not None:
                if response["nodeIds"][x] in node_ids:
                    spikes.simulation_times.append(response["simulationTimes"][x])
                    spikes.node_ids.append(response["nodeIds"][x])
            else:
                spikes.simulation_times.append(response["simulationTimes"][x])
                spikes.node_ids.append(response["nodeIds"][x])

    # sort
    sorted_ids = [x for _, x in sorted(
        zip(spikes.simulation_times, spikes.node_ids))]
    spikes.node_ids = sorted_ids
    spikes.simulation_times.sort()

    # offset and limit
    if (skip is None):
        skip = 0
    if (top is None or (top + skip) > len(spikes.node_ids)):
        top = len(spikes.node_ids) - skip
    spikes.node_ids = spikes.node_ids[skip:skip+top]
    spikes.simulation_times = spikes.simulation_times[skip:skip+top]

    return spikes


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
    for node in simulation_nodes.nest_simulation_nodes:
        response = requests.get(
            node+"/spikes", params={"fromTime": from_time, "toTime": to_time, "nodeCollectionId": node_collection_id}).json()
        for x in range(len(response["simulationTimes"])):
            spikes.simulation_times.append(response["simulationTimes"][x])
            spikes.node_ids.append(response["nodeIds"][x])

    # sort
    sorted_ids = [x for _, x in sorted(
        zip(spikes.simulation_times, spikes.node_ids))]
    spikes.node_ids = sorted_ids
    spikes.simulation_times.sort()

    # offset and limit
    if (skip is None):
        skip = 0
    if (top is None or (top + skip) > len(spikes.node_ids)):
        top = len(spikes.node_ids) - skip
    spikes.node_ids = spikes.node_ids[skip:skip+top]
    spikes.simulation_times = spikes.simulation_times[skip:skip+top]

    return spikes


def nest_get_spikes_by_spikedetector(spikedetector_id, from_time=None, to_time=None, node_ids=None, skip=None, top=None):  # noqa: E501
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
    for node in simulation_nodes.nest_simulation_nodes:
        response = requests.get(
            node+"/spikes", params={"fromTime": from_time, "toTime": to_time, "nodeIds": node_ids, "spikedetectorId": spikedetector_id}).json()
        for x in range(len(response["simulationTimes"])):
            if node_ids is not None:
                if response["nodeIds"][x] in node_ids:
                    spikes.simulation_times.append(response["simulationTimes"][x])
                    spikes.node_ids.append(response["nodeIds"][x])
            else:
                spikes.simulation_times.append(response["simulationTimes"][x])
                spikes.node_ids.append(response["nodeIds"][x])

    # sort
    sorted_ids = [x for _, x in sorted(
        zip(spikes.simulation_times, spikes.node_ids))]
    spikes.node_ids = sorted_ids
    spikes.simulation_times.sort()

    # offset and limit
    if (skip is None):
        skip = 0
    if (top is None or (top + skip) > len(spikes.node_ids)):
        top = len(spikes.node_ids) - skip
    spikes.node_ids = spikes.node_ids[skip:skip+top]
    spikes.simulation_times = spikes.simulation_times[skip:skip+top]

    return spikes
