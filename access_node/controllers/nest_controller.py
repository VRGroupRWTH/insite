import connexion
import six

from access_node.models.neuron_properties import NeuronProperties  # noqa: E501
from access_node.models.simulation_time_info import SimulationTimeInfo  # noqa: E501
from access_node.models.spikes import Spikes  # noqa: E501
from access_node import util

from access_node.models.nodes import nodes
import requests


def get_gids():  # noqa: E501
    """Retrieves the list of all GID.

     # noqa: E501


    :rtype: List[int]
    """
    gids = requests.get(nodes.info_node+'/gids').json()
    return gids


def get_gids_in_population(population_id):  # noqa: E501
    """Retrieves the list of all neuron IDs.

     # noqa: E501

    :param population_id: The identifier of the population
    :type population_id: int

    :rtype: List[int]
    """
    gids = requests.get(nodes.info_node+'/population/$' +
                        str(population_id)+'/gids').json()
    return gids


def get_neuron_properties(gids=None):  # noqa: E501
    """Retrieves the properties of the specified neurons.

     # noqa: E501

    :param gids: A list of GIDs queried for properties.
    :type gids: List[int]

    :rtype: List[NeuronProperties]
    """
    properties = requests.get(nodes.info_node+'/neuron_properties').json()
    return properties


def get_populations():  # noqa: E501
    """Retrieves the list of all population IDs.

     # noqa: E501


    :rtype: List[int]
    """
    populations = requests.get(nodes.info_node+'/populations').json()
    return populations


def get_simulation_time_info():  # noqa: E501
    """Retrieves simulation time information.

     # noqa: E501


    :rtype: SimulationTimeInfo
    """
    time_info = requests.get(nodes.info_node+'/simulation_time_info').json()
    return time_info


def get_spikes(_from=None, to=None, gids=None, offset=None, limit=None):  # noqa: E501
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
    for node in nodes.simulation_nodes:
        response = requests.get(
            'http://'+node+'/spikes', params={"_from": _from, "to": to, "gids": gids}).json()
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
    if (limit is None):
        limit = len(spikes.gids)
    spikes.gids = spikes.gids[offset:limit]
    spikes.simulation_times = spikes.simulation_times[offset:limit]

    return spikes


def get_spikes_by_population(population_id, _from=None, to=None, offset=None, limit=None):  # noqa: E501
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
    for node in nodes.simulation_nodes:
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
    if (limit is None):
        limit = len(spikes.gids)
    spikes.gids = spikes.gids[offset:limit]
    spikes.simulation_times = spikes.simulation_times[offset:limit]

    return spikes
