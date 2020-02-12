import connexion
import six

from access_node.models.arbor_cell_properties import ArborCellProperties  # noqa: E501
from access_node.models.arbor_measurement import ArborMeasurement  # noqa: E501
from access_node.models.probe import Probe  # noqa: E501
from access_node.models.simulation_time_info import SimulationTimeInfo  # noqa: E501
from access_node.models.spikes import Spikes  # noqa: E501
from access_node import util

from access_node.models.nodes import nodes
import requests

def arbor_get_attributes():  # noqa: E501
    """Retrieves the list of all attributes.

     # noqa: E501


    :rtype: List[str]
    """
    return 'do some magic!'


def arbor_get_cell_ids():  # noqa: E501
    """Retrieves the list of all cell ids.

     # noqa: E501


    :rtype: List[int]
    """
    cell_ids = requests.get(nodes.info_node+'/arbor/cell_ids').json()
    return cell_ids


def arbor_get_cell_properties(cell_ids=None):  # noqa: E501
    """Retrieves the properties of the specified cells.

     # noqa: E501

    :param cell_ids: A list of cell IDs queried for properties.
    :type cell_ids: List[int]

    :rtype: List[ArborCellProperties]
    """
    cell_properties = requests.get(nodes.info_node+'/arbor/cell_properties').json()
    return cell_properties


def arbor_get_measurements(attribute, probe_ids=None, _from=None, to=None, offset=None, limit=None):  # noqa: E501
    """Retrieves the measurements for given probes (optional).

     # noqa: E501

    :param attribute: The attribute to query (e.g., &#39;V_m&#39; for the membrane potential)
    :type attribute: str
    :param probe_ids: A list of probes ids queried for data.
    :type probe_ids: List[int]
    :param _from: The start time (including) to be queried.
    :type _from: float
    :param to: The end time (excluding) to be queried.
    :type to: float
    :param offset: The offset into the result.
    :type offset: int
    :param limit: The maximum of entries to be result.
    :type limit: int

    :rtype: ArborMeasurement
    """
    if measurement_point_ids == None:
        measurement_points = arbor_get_measurement_points()
        measurement_point_ids = []
        for point in measurement_points:
               measurement_point_ids.append(point.id)

    init = True
    sim_times = []
    measurement = ArborMeasurement([], [], [])
    for node in nodes.arbor_simulation_nodes:
        response = requests.get(
            'http://'+node+'/arbor/measurements', params={"attribute": attribute, "measurement_point_ids": measurement_point_ids, "_from": _from, "to": to, "gids": gids}).json()
        if init:
            sim_times = response['simulation_times']
            measurement = ArborMeasurement(
                sim_times, measurement_point_ids, [None for x in range(0, (len(sim_times)*len(measurement_point_ids)))])
            init = False
        for x in range(len(response['measurement_point_ids'])):
            m_id = response['measurement_point_ids'][x]
            index = measurement.measurement_point_ids.index(m_id)
            index_offset = index * len(sim_times)
            for y in range(len(sim_times)):
                measurement.values[index_offset +
                                   y] = response['values'][x*len(sim_times)+y]

    # offset and limit
    if (offset is None):
        offset = 0
    if (limit is None or (limit + offset) > len(measurement.measurement_point_ids)):
        limit = len(measurement.measurement_point_ids) - offset
    measurement.measurement_point_ids = measurement.measurement_point_ids[offset:offset+limit]
    measurement.values = measurement.values[offset *
                                            len(sim_times):(offset+limit)*len(sim_times)]

    return measurement


def arbor_get_probes(attribute=None):  # noqa: E501
    """Retrieves the list of all probes for a given attribute (optional).

     # noqa: E501

    :param attribute: The attribute measured for which existing probes will be returned.
    :type attribute: str

    :rtype: List[Probe]
    """
    probes = requests.get(nodes.info_node+'/arbor/probes').json()
    return probes


def arbor_get_simulation_time_info():  # noqa: E501
    """Retrieves simulation time information.

     # noqa: E501


    :rtype: SimulationTimeInfo
    """
    simulation_time_info = requests.get(nodes.info_node+'/arbor/simulation_time_info').json()
    return simulation_time_info


def arbor_get_spikes(_from=None, to=None, cell_ids=None, segment_ids=None, offset=None, limit=None):  # noqa: E501
    """Retrieves the spikes for the given simulation times (optional),  cell and segment (optional).

     # noqa: E501

    :param _from: The start time (including) to be queried.
    :type _from: float
    :param to: The end time (excluding) to be queried.
    :type to: float
    :param cell_ids: A list of cell ids queried for spike data.
    :type cell_ids: List[int]
    :param segment_ids: A list of segment ids queried for spike data.
    :type segment_ids: List[int]
    :param offset: The offset into the result.
    :type offset: int
    :param limit: The maximum of entries to be result.
    :type limit: int

    :rtype: Spikes
    """
    spikes = Spikes([], [])
    for node in nodes.arbor_simulation_nodes:
        response = requests.get(
            'http://'+node+'/arbor/spikes', params={"from": _from, "to": to, "gids": gids}).json()
        for x in range(len(response['simulation_times'])):
            spikes.simulation_times.append(response['simulation_times'][x])
            spikes.gids.append(response['gids'][x])

    # sort
    sorted_ids = [x for _, x in sorted(
        zip(spikes.simulation_times, spikes.gids))]
    spikes.gids = sorted_ids
    spikes.simulation_times.sort()

    # offset and limit
    if (offset is None):
        offset = 0
    if (limit is None or (limit + offset) > len(spikes.gids)):
        limit = len(spikes.gids) - offset
    spikes.gids = spikes.gids[offset:offset+limit]
    spikes.simulation_times = spikes.simulation_times[offset:offset+limit]

    return spikes
