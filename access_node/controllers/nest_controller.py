import connexion
import six

from access_node.models.attribute import Attribute  # noqa: E501
from access_node.models.data import Data  # noqa: E501
from access_node.models.error import Error  # noqa: E501
from access_node.models.spikes import Spikes  # noqa: E501
from access_node import util

# My imports
import json
import requests


def get_attributes():  # noqa: E501
    """Retrieves the details of per-neuron attributes.

     # noqa: E501


    :rtype: List[Attribute]
    """
    with open('access_node//distribution_nodes.json', 'r') as f:
        dist_nodes = json.load(f)
    information_node = dist_nodes['addresses'][0]

    response = requests.get(information_node+'/attributes').json()
    return response


def get_data(attribute, simulation_steps=None, neuron_ids=None):  # noqa: E501
    """Retrieves the per-neuron attributes for the given attribute name, simulation steps (optional) and neuron IDs (optional).

     # noqa: E501

    :param attribute: Attribute name.
    :type attribute: str
    :param simulation_steps: Simulation steps (leave empty for all steps).
    :type simulation_steps: List[]
    :param neuron_ids: Neuron IDs (leave empty for all neurons).
    :type neuron_ids: List[]

    :rtype: Data
    """
    # Replace this with request to information_node
    with open('access_node//distribution_nodes.json', 'r') as f:
        dist_nodes = json.load(f)
    addresses = dist_nodes['addresses']

    if simulation_steps != None:
        first_ts = simulation_steps[0]
        last_ts = simulation_steps[-1]
        num_ts = 
    else:
        first_ts = get_timestep()
        last_ts = first_ts * get_simulation_step_count()
        count = get_simulation_step_count()
        timesteps = [first_ts*x for x in range(count)]

    if neuron_ids != None:
        ids = neuron_ids
    else:
        ids = get_neuron_ids()

    data = Data([timesteps[0], timesteps[-1]], ids,
                [[None]*len(timesteps) for x in range(len(ids))])
    for address in addresses:
        response = requests.get(address+'/data', params={
                                "attribute": attribute, "simulation_steps": simulation_steps, "neuron_ids": neuron_ids}).json()
        for x in range(len(response['neuron_ids'])):
            data.values[data.neuron_ids.index(
                response['neuron_ids'][x])] = response['data'][x]
    return data


def get_neuron_ids():  # noqa: E501
    """Retrieves the list of all neuron IDs.

     # noqa: E501


    :rtype: List[float]
    """
    with open('access_node//distribution_nodes.json', 'r') as f:
        dist_nodes = json.load(f)
    information_node = dist_nodes['addresses'][0]

    response = requests.get(information_node+'/neuron_ids').json()
    return response


def get_simulation_step_count():  # noqa: E501
    """Retrieves the number of simulation steps.

     # noqa: E501


    :rtype: float
    """
    with open('access_node//distribution_nodes.json', 'r') as f:
        dist_nodes = json.load(f)
    information_node = dist_nodes['addresses'][0]

    response = requests.get(information_node+'/simulation_step_count').json()
    return response


def get_spikes(simulation_steps=None, neuron_ids=None):  # noqa: E501
    """Retrieves the spikes for the given simulation steps (optional) and neuron IDs (optional).

     # noqa: E501

    :param simulation_steps: Simulation steps (leave empty for all steps).
    :type simulation_steps: List[]
    :param neuron_ids: Neuron IDs (leave empty for all neurons).
    :type neuron_ids: List[]

    :rtype: Spikes
    """
    # Replace this with request to information_node
    with open('access_node//distribution_nodes.json', 'r') as f:
        dist_nodes = json.load(f)
    addresses = dist_nodes['addresses']

    spikes = Spikes([], [])
    for address in addresses:
        response = requests.get(
            address+'/spikes', params={"simulation_steps": simulation_steps, "neuron_ids": neuron_ids}).json()
        for x in range(len(response['simulation_steps'])):
            spikes.simulation_steps.append(response['simulation_steps'][x])
            spikes.neuron_ids.append(response['neuron_ids'][x])

    # Sort this?
    return spikes


def get_timestep():  # noqa: E501
    """Retrieves the time between two simulation steps.

     # noqa: E501


    :rtype: float
    """
    with open('access_node//distribution_nodes.json', 'r') as f:
        dist_nodes = json.load(f)
    information_node = dist_nodes['addresses'][0]

    response = requests.get(information_node+'/timestep').json()
    return response
