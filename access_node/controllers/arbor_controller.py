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
import psycopg2
import numpy as np


def connect_to_database():
    database_host = 'database'
    with open('database_host.txt') as database_host_file:
        database_host = database_host_file.readline().rstrip('\n')
    return psycopg2.connect(database="postgres", user="postgres",
                       password="postgres", host=database_host, port="5432")


def arbor_get_attributes():  # noqa: E501
    """Retrieves the list of all attributes.

     # noqa: E501


    :rtype: List[str]
    """
    con = connect_to_database()
    cur = con.cursor()

    cur.execute("SELECT name FROM arbor_attribute")
    attributes = [i[0] for i in cur.fetchall()]

    con.close()
    return attributes


def arbor_get_cell_ids():  # noqa: E501
    """Retrieves the list of all cell ids.

     # noqa: E501


    :rtype: List[int]
    """
    con = connect_to_database()
    cur = con.cursor()

    cur.execute("SELECT id FROM arbor_cell")
    cell_ids = [i[0] for i in cur.fetchall()]

    con.close()
    return cell_ids


def arbor_get_cell_properties(cell_ids=None):  # noqa: E501
    """Retrieves the properties of the specified cells.

     # noqa: E501

    :param cell_ids: A list of cell IDs queried for properties.
    :type cell_ids: List[int]

    :rtype: List[ArborCellProperties]
    """
    con = connect_to_database()
    cur = con.cursor()

    if cell_ids == None:
        cur.execute("SELECT cell_id, property FROM cell_property")
    else: 
        cur.execute("SELECT cell_id, property FROM cell_property WHERE cell_property.cell_id IN %s", (tuple(cell_ids),))

    properties = np.array(cur.fetchall())
    cell_ids = np.unique(properties[:,0])
    cell_properties = []
    for i in range(len(cell_ids)):
        per_cell_properties = []
        for prop in properties:
            if prop[0] == cell_ids[i]:
                per_cell_properties.append(prop)
        cell_properties.append(ArborCellProperties(cell_ids[i], per_cell_properties))

    con.close()
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
    if probe_ids == None:
        probes = arbor_get_probes()
        probe_ids = []
        for probe in probes:
               probe_ids.append(probe.id)

    init = True
    sim_times = []
    measurement = ArborMeasurement([], [], [])
    for node in nodes.arbor_simulation_nodes:
        response = requests.get(
            'http://'+node+'/arbor/measurements', params={"attribute": attribute, "probe_ids": probe_ids, "_from": _from, "to": to}).json()
        if init:
            sim_times = response['simulation_times']
            measurement = ArborMeasurement(
                sim_times, probe_ids, [None for x in range(0, (len(sim_times)*len(probe_ids)))])
            init = False
        for x in range(len(response['probe_ids'])):
            m_id = response['probe_ids'][x]
            index = measurement.probe_ids.index(m_id)
            index_offset = index * len(sim_times)
            for y in range(len(sim_times)):
                measurement.values[index_offset +
                                   y] = response['values'][x*len(sim_times)+y]

    # offset and limit
    if (offset is None):
        offset = 0
    if (limit is None or (limit + offset) > len(measurement.probe_ids)):
        limit = len(measurement.probe_ids) - offset
    measurement.probe_ids = measurement.probe_ids[offset:offset+limit]
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
    con = connect_to_database()
    cur = con.cursor()

    if attribute == None:
        cur.execute('''SELECT 
                        arbor_probe.id, 
                        arbor_probe.cell_id, 
                        arbor_probe.segment_id,  
                        arbor_probe.position,
                        FROM arbor_probe''')
    else:
        cur.execute('''SELECT 
                        arbor_probe.id, 
                        arbor_probe.cell_id, 
                        arbor_probe.segment_id,  
                        arbor_probe.position,
                        FROM arbor_probe INNER JOIN arbor_attribute
                        ON arbor_probe.attribute_id = arbor_attribute.id
                        WHERE arbor_attribute.name = %s''', (attribute,))

    probes = np.array(cur.fetchall())
    con.close()

    return  [Probe(*probe) for probe in probes]


def arbor_get_simulation_time_info():  # noqa: E501
    """Retrieves simulation time information.

     # noqa: E501


    :rtype: SimulationTimeInfo
    """
    con = connect_to_database()
    cur = con.cursor()

    cur.execute("SELECT MIN(current_simulation_time) FROM arbor_simulation_node")
    current_time = cur.fetchall()[0][0]

    con.close()

    # TODO Add Start and End time when available
    time_info = SimulationTimeInfo(current=current_time)
    return time_info


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
            'http://'+node+'/arbor/spikes', params={"from": _from, "to": to, "cell_ids": cell_ids, "segment_ids": segment_ids}).json()
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
