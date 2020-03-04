import connexion
import six

from access_node.models.multimeter_info import MultimeterInfo  # noqa: E501
from access_node.models.multimeter_measurement import MultimeterMeasurement  # noqa: E501
from access_node.models.nest_neuron_properties import NestNeuronProperties  # noqa: E501
from access_node.models.simulation_time_info import SimulationTimeInfo  # noqa: E501
from access_node.models.spikes import Spikes  # noqa: E501
from access_node import util

from access_node.models.nodes import nodes
import requests
import psycopg2
import numpy as np

def nest_get_gids():  # noqa: E501
    """Retrieves the list of all GID.

     # noqa: E501


    :rtype: List[int]
    """
    con = psycopg2.connect(database="postgres", user="postgres",
                       password="docker", host="database", port="5432")
    
    cur = con.cursor()
    cur.execute("SELECT GID FROM GIDS")
    gids = [i[0] for i in cur.fetchall()]
    con.close()
    return gids


def nest_get_gids_in_population(population_id):  # noqa: E501
    """Retrieves the list of all neuron IDs within the population.

     # noqa: E501

    :param population_id: The identifier of the population
    :type population_id: int

    :rtype: List[int]
    """
    con = psycopg2.connect(database="postgres", user="postgres",
                       password="docker", host="database", port="5432")
    
    cur = con.cursor()
    cur.execute("SELECT GID FROM GIDS WHERE GIDS.POPULATION_ID ="+str(population_id))
    gids = [i[0] for i in cur.fetchall()]
    con.close()
    return gids


def nest_get_multimeter_info():  # noqa: E501
    """Retreives the available multimeters and their properties.
    
     # noqa: E501


    :rtype: MultimeterInfo
    """
    con = psycopg2.connect(database="postgres", user="postgres",
                       password="docker", host="database", port="5432")
    cur = con.cursor()

    cur.execute("SELECT MULTIMETER_ID FROM MULTIMETERS")
    mult_ids = cur.fetchall()

    cur.execute("SELECT regexp_replace(ATTRIBUTE, '\s+$', '') FROM (SELECT * FROM MULTIMETERS) AS MULT_INFO;")
    attributes = cur.fetchall()
    
    gids = []
    for id in mult_ids:
        cur.execute("SELECT GID FROM MULT_PER_GID WHERE MULTIMETER_ID = %s", (id,))
        gids.append([i[0] for i in cur.fetchall()])

    mult_info = []
    for i in range(len(mult_ids)):
        mult_info.append({"id": mult_ids[i][0],
                    "attributes": attributes[i][0],
                    "gids": gids[i]})

    con.close()

    return mult_info


def nest_get_multimeter_measurements(multimeter_id, attribute, _from=None, to=None, gids=None, offset=None, limit=None):  # noqa: E501
    """Retrieves the measurements for a multimeter, attribute and GIDS (optional).

     # noqa: E501

    :param multimeter_id: The multimeter to query
    :type multimeter_id: 
    :param attribute: The attribute to query (e.g., &#39;V_m&#39; for the membrane potential)
    :type attribute: str
    :param _from: The start time (including) to be queried.
    :type _from: float
    :param to: The end time (excluding) to be queried.
    :type to: float
    :param gids: A list of GIDs queried for spike data.
    :type gids: List[int]
    :param offset: The offset into the result.
    :type offset: int
    :param limit: The maximum of entries to be result.
    :type limit: int

    :rtype: MultimeterMeasurement
    """
    mult_info = nest_get_multimeter_info()

    mult_gids = []
    multimeter_exists = False
    for mult in mult_info:
        if mult['id'] == multimeter_id:
            multimeter_exists = True
            if attribute not in mult['attributes']:
                return Status(code=400, message="Given multimeter does not measure given attribute")
            mult_gids = mult['gids']
            break
    if not multimeter_exists:
        return Status(code=400, message="Given multimeter does not exist")

    if gids == None:
        gids = mult_gids
    else:
        for gid in gids:
            if gid not in mult_gids:
                return Status(code=400, message="Gid "+str(gid)+" is not measured by given Multimeter")

    init = True
    sim_times = []
    measurement = MultimeterMeasurement([], [], [])
    for node in nodes.nest_simulation_nodes:
        response = requests.get(
            'http://'+node+'/multimeter_measurement', params={"multimeter_id": multimeter_id, "attribute": attribute, "_from": _from, "to": to, "gids": gids}).json()
        if init:
            sim_times = response['simulation_times']
            measurement = MultimeterMeasurement(
                sim_times, gids, [None for x in range(0, (len(sim_times)*len(gids)))])
            init = False
        for x in range(len(response['gids'])):
            gid = response['gids'][x]
            index = measurement.gids.index(gid)
            index_offset = index * len(sim_times)
            for y in range(len(sim_times)):
                measurement.values[index_offset +
                                   y] = response['values'][x*len(sim_times)+y]

    # offset and limit
    if (offset is None):
        offset = 0
    if (limit is None or (limit + offset) > len(measurement.gids)):
        limit = len(measurement.gids) - offset
    measurement.gids = measurement.gids[offset:offset+limit]
    measurement.values = measurement.values[offset *
                                            len(sim_times):(offset+limit)*len(sim_times)]

    return measurement


def nest_get_neuron_properties(gids=None):  # noqa: E501
    """Retrieves the properties of the specified neurons.

     # noqa: E501

    :param gids: A list of GIDs queried for properties.
    :type gids: List[int]

    :rtype: List[NestNeuronProperties]
    """
    con = psycopg2.connect(database="postgres", user="postgres",
                       password="docker", host="database", port="5432")
    
    cur = con.cursor()

    cur.execute("Select * FROM GIDS LIMIT 0")
    colnames = np.array([desc[0] for desc in cur.description])
    # column 1 and 2 contain the Node_id/Population_id and thus are removed
    colnames = np.delete(colnames, [1,2])

    if gids == None:
        cur.execute("Select * FROM GIDS")
    else:
        cur.execute("Select * FROM GIDS WHERE GID IN %s", (tuple(gids),))
    con.close()

    properties = np.array(cur.fetchall())
    properties = np.delete(properties, [1,2], 1)

    nest_properties = []
    for k in range(len(properties[:,0])):
        props = {}
        id = properties[k,0]
        for i in range(1, len(colnames)):
            props.update({colnames[i]: properties[k,i]})
        nest_properties.extend(NestNeuronProperties(id, props))

    return nest_properties


def nest_get_populations():  # noqa: E501
    """Retrieves the list of all population IDs.

     # noqa: E501


    :rtype: List[int]
    """
    con = psycopg2.connect(database="postgres", user="postgres",
                       password="docker", host="database", port="5432")
    
    cur = con.cursor()
    cur.execute("SELECT DISTINCT (POPULATION_ID) FROM GIDS")
    populations = [i[0] for i in cur.fetchall()]
    return populations


def nest_get_simulation_time_info():  # noqa: E501
    """Retrieves simulation time information.

     # noqa: E501


    :rtype: SimulationTimeInfo
    """
    con = psycopg2.connect(database="postgres", user="postgres",
                       password="docker", host="database", port="5432")
    
    cur = con.cursor()
    cur.execute("SELECT MIN(CURRENT_SIM_TIME) FROM SIMULATION_NODES")
    current_time = cur.fetchall()[0][0]

    # TODO Add Start and End time when available
    time_info = SimulationTimeInfo(current=current_time)
    return time_info


def nest_get_spikes(_from=None, to=None, gids=None, offset=None, limit=None):  # noqa: E501
    """Retrieves the spikes for the given simulation steps (optional) and GIDS (optional).

     # noqa: E501

    :param _from: The start time (including) to be queried.
    :type _from: float
    :param to: The end time (excluding) to be queried.
    :type to: float
    :param gids: A list of GIDs queried for spike data.
    :type gids: List[int]
    :param offset: The offset into the result.
    :type offset: int
    :param limit: The maximum of entries to be result.
    :type limit: int

    :rtype: Spikes
    """
    spikes = Spikes([], [])
    for node in nodes.nest_simulation_nodes:
        response = requests.get(
            'http://'+node+'/spikes', params={"from": _from, "to": to, "gids": gids}).json()
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


def nest_get_spikes_by_population(population_id, _from=None, to=None, offset=None, limit=None):  # noqa: E501
    """Retrieves the spikes for the given simulation steps (optional) and population.

     # noqa: E501

    :param population_id: The identifier of the population.
    :type population_id: int
    :param _from: The start time (including) to be queried.
    :type _from: float
    :param to: The end time (excluding) to be queried.
    :type to: float
    :param offset: The offset into the result.
    :type offset: int
    :param limit: The maximum of entries to be result.
    :type limit: int

    :rtype: Spikes
    """
    spikes = Spikes([], [])
    for node in nodes.nest_simulation_nodes:
        response = requests.get(
            node+'/population/'+population_id+'/spikes', params={"from": _from, "to": to}).json()
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
    