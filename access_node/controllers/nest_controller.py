import connexion
import six

from access_node.models.neuron_properties import NeuronProperties  # noqa: E501
from access_node.models.simulation_time_info import SimulationTimeInfo  # noqa: E501
from access_node.models.spikes import Spikes  # noqa: E501
from access_node import util

import requests


def get_gids():  # noqa: E501
    """Retrieves the list of all GID.

     # noqa: E501


    :rtype: List[float]
    """
    return 'do some magic!'


def get_gids_in_population(population_id):  # noqa: E501
    """Retrieves the list of all neuron IDs.

     # noqa: E501

    :param population_id: The identifier of the population
    :type population_id: str

    :rtype: List[float]
    """
    return 'do some magic!'


def get_neuron_properties(gids=None):  # noqa: E501
    """Retrieves the properties of the specified neurons.

     # noqa: E501

    :param gids: A list of GIDs queried for properties.
    :type gids: List[]

    :rtype: List[NeuronProperties]
    """
    return 'do some magic!'


def get_populations():  # noqa: E501
    """Retrieves the list of all populations.

     # noqa: E501


    :rtype: List[str]
    """
    return 'do some magic!'


def get_simulation_step_count():  # noqa: E501
    """Retrieves the number of simulation steps.

     # noqa: E501


    :rtype: SimulationTimeInfo
    """
    return 'do some magic!'


def get_spikes(_from=None, to=None, gids=None, offset=None, limit=None):  # noqa: E501
    """Retrieves the spikes for the given simulation steps (optional) and GIDS (optional).

     # noqa: E501

    :param _from: The start time (including) to be queried.
    :type _from: 
    :param to: The end time (excluding) to be queried.
    :type to: 
    :param gids: A list of GIDs queried for spike data.
    :type gids: List[]
    :param offset: The offset into the result.
    :type offset: 
    :param limit: The maximum of entries to be result.
    :type limit: 

    :rtype: Spikes
    """
    with open('access_node//distribution_nodes.json', 'r') as f:
        dist_nodes = json.load(f)
    simulation_nodes = dist_nodes['addresses']

    spikes = Spikes([], [])
    for node in simulation_nodes:
        response = requests.get(
            node+'/spikes', params={"_from": _from, "to": to, "gids": gids}).json()
        for x in range(len(response['simulation_steps'])):
            spikes.simulation_steps.append(response['simulation_steps'][x])
            spikes.neuron_ids.append(response['neuron_ids'][x])

    # sort
    sorted_ids = [x for _,x in sorted(zip(spikes.simulation_steps, spikes.neuron_ids))]
    spikes.neuron_ids = sorted_ids
    spikes.simulation_steps.sort()

    # offset and limit
    if (offset is None):
         offset = 0
    if (limit is None):
         limit = len(spikes.neuron_ids)
    spikes.neuron_ids = spikes.neuron_ids[offset:limit]
    spikes.simulation_steps = spikes.simulation_steps[offset:limit]

    return spikes


def get_spikes_by_population(population_id, _from=None, to=None, offset=None, limit=None):  # noqa: E501
    """Retrieves the spikes for the given simulation steps (optional) and population.

     # noqa: E501

    :param population_id: The identifier of the population.
    :type population_id: str
    :param _from: The start time (including) to be queried.
    :type _from: 
    :param to: The end time (excluding) to be queried.
    :type to: 
    :param offset: The offset into the result.
    :type offset: 
    :param limit: The maximum of entries to be result.
    :type limit: 

    :rtype: Spikes
    """
    with open('access_node//distribution_nodes.json', 'r') as f:
        dist_nodes = json.load(f)
    simulation_nodes = dist_nodes['addresses']

    spikes = Spikes([], [])
    for node in simulation_nodes:
        response = requests.get(
            node+'/population/'+population_id+'/spikes', params={"_from": _from, "to": to}).json()
        for x in range(len(response['simulation_steps'])):
            spikes.simulation_steps.append(response['simulation_steps'][x])
            spikes.neuron_ids.append(response['neuron_ids'][x])

    # sort
    sorted_ids = [x for _,x in sorted(zip(spikes.simulation_steps, spikes.neuron_ids))]
    spikes.neuron_ids = sorted_ids
    spikes.simulation_steps.sort()

    # offset and limit
    if (offset is None):
         offset = 0
    if (limit is None):
         limit = len(spikes.neuron_ids)
    spikes.neuron_ids = spikes.neuron_ids[offset:limit]
    spikes.simulation_steps = spikes.simulation_steps[offset:limit]

    return spikes

