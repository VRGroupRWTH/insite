import connexion
import six

from access_node.models.error_response import ErrorResponse  # noqa: E501
from access_node.models.multimeter_info import MultimeterInfo  # noqa: E501
from access_node.models.multimeter_measurement import MultimeterMeasurement  # noqa: E501
from access_node.models.nest_node_collection_properties import NestNodeCollectionProperties  # noqa: E501
from access_node.models.nest_node_properties import NestNodeProperties  # noqa: E501
from access_node.models.simulation_time_info import SimulationTimeInfo  # noqa: E501
from access_node.models.spikedetector_info import SpikedetectorInfo  # noqa: E501
from access_node.models.spikes import Spikes  # noqa: E501
from access_node import util


def nest_get_kernel_status():  # noqa: E501
    """Retreives the current status of the NEST kernel.

     # noqa: E501


    :rtype: List[MultimeterInfo]
    """
    return 'do some magic!'


def nest_get_multimeter_by_id(multimeter_id):  # noqa: E501
    """Retreives the available multimeters and their properties.

     # noqa: E501

    :param multimeter_id: The identifier of the multimeter.
    :type multimeter_id: int

    :rtype: MultimeterInfo
    """
    return 'do some magic!'


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
    return 'do some magic!'


def nest_get_multimeters():  # noqa: E501
    """Retreives the available multimeters and their properties.

     # noqa: E501


    :rtype: List[MultimeterInfo]
    """
    return 'do some magic!'


def nest_get_node_by_id(node_id):  # noqa: E501
    """Retrieves the properties of the specified node.

     # noqa: E501

    :param node_id: The ID of the queried node.
    :type node_id: int

    :rtype: NestNodeProperties
    """
    return 'do some magic!'


def nest_get_node_collections():  # noqa: E501
    """Retrieves a list of all node collection IDs.

     # noqa: E501


    :rtype: List[NestNodeCollectionProperties]
    """
    return 'do some magic!'


def nest_get_node_ids():  # noqa: E501
    """Retrieves a list of all node IDs.

     # noqa: E501


    :rtype: List[int]
    """
    return 'do some magic!'


def nest_get_node_ids_by_node_collection(node_collection_id):  # noqa: E501
    """Retrieves the list of all node ids within the node collection.

     # noqa: E501

    :param node_collection_id: The identifier of the node collection
    :type node_collection_id: int

    :rtype: List[int]
    """
    return 'do some magic!'


def nest_get_nodes():  # noqa: E501
    """Retrieves all nodes of the simulation.

     # noqa: E501


    :rtype: List[NestNodeProperties]
    """
    return 'do some magic!'


def nest_get_nodes_by_node_collection(node_collection_id):  # noqa: E501
    """Retrieves the list of all node within the node collection.

     # noqa: E501

    :param node_collection_id: The identifier of the node collection
    :type node_collection_id: int

    :rtype: List[NestNodeProperties]
    """
    return 'do some magic!'


def nest_get_simulation_time_info():  # noqa: E501
    """Retrieves simulation time information (stepSize, begin, current, end).

     # noqa: E501


    :rtype: SimulationTimeInfo
    """
    return 'do some magic!'


def nest_get_spikedetectors():  # noqa: E501
    """Queries all spike detectors accessable to the pipeline.

     # noqa: E501


    :rtype: SpikedetectorInfo
    """
    return 'do some magic!'


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
    return 'do some magic!'


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
    return 'do some magic!'


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
    return 'do some magic!'
