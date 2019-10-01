import connexion
import six

from access_node.models.error import Error  # noqa: E501
from access_node.models.info import Info  # noqa: E501
from access_node.models.multimeter import Multimeter  # noqa: E501
from access_node.models.multimeter_info import MultimeterInfo  # noqa: E501
from access_node.models.spikes import Spikes  # noqa: E501
from access_node import util


def get_current_simulation_time():  # noqa: E501
    """Retrieves the current simulation time.

     # noqa: E501


    :rtype: float
    """
    return 'do some magic!'


def get_data(multimeter, attribute, _from=None, to=None, neuron_ids=None):  # noqa: E501
    """Retrieves the per-neuron attributes for the given multimeter name, attribute name, duration (from - to) (optional) and neuron IDs (optional).

     # noqa: E501

    :param multimeter: Multimeter name.
    :type multimeter: str
    :param attribute: Attribute name.
    :type attribute: str
    :param _from: First timestep to retreive (leave empty for first step possible).
    :type _from: float
    :param to: Last timestep to retreive (leave empty for last step possible).
    :type to: float
    :param neuron_ids: Neuron IDs (leave empty for all neurons).
    :type neuron_ids: List[]

    :rtype: Multimeter
    """
    with open('access_node//distribution_nodes.json', 'r') as f:
        dist_nodes = json.load(f)
    addresses = dist_nodes['addresses']

    if neuron_ids != None:
        ids = neuron_ids
    else:
        ids = get_neuron_ids()

    # TODO Query this
    interval = 0.1

    num_values_per_neuron = (to-_from)/interval


    mult_response = Multimeter(multimeter)
    mult_response._from = _from
    mult_response.to = to
    mult_response.neuron_ids = ids
    mult_response.interval = interval
    mult_response.values = [[None]*num_values_per_neuron for x in range(len(ids))]

    for address in addresses:
        response = requests.get(address+'/data', params={ "multimeter": multimeter,
                                "attribute": attribute, "from": _from, "to": to, "neuron_ids": neuron_ids}).json()
        for x in range(len(response['neuron_ids'])):
            mult_response.values[mult_response.neuron_ids.index(
                response['neuron_ids'][x])] = response['values'][x]
    return mult_response


def get_info():  # noqa: E501
    """Retrieves general simulation information.

     # noqa: E501


    :rtype: Info
    """
    return 'do some magic!'


def get_multimeters():  # noqa: E501
    """Retrieves the details of all multimeters.

     # noqa: E501


    :rtype: List[MultimeterInfo]
    """
    return 'do some magic!'


def get_neuron_ids():  # noqa: E501
    """Retrieves the list of all neuron IDs.

     # noqa: E501


    :rtype: List[float]
    """
    return 'do some magic!'


def get_spikes(simulation_steps=None, neuron_ids=None):  # noqa: E501
    """Retrieves the spikes for the given simulation steps (optional) and neuron IDs (optional).

     # noqa: E501

    :param simulation_steps: Simulation steps (leave empty for all steps).
    :type simulation_steps: List[]
    :param neuron_ids: Neuron IDs (leave empty for all neurons).
    :type neuron_ids: List[]

    :rtype: Spikes
    """
    return 'do some magic!'
