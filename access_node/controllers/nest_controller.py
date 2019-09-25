import connexion
import six

from access_node.models.attribute import Attribute  # noqa: E501
from access_node.models.data import Data  # noqa: E501
from access_node.models.error import Error  # noqa: E501
from access_node.models.spikes import Spikes  # noqa: E501
from access_node import util


def get_attributes():  # noqa: E501
    """Retrieves the details of per-neuron attributes.

     # noqa: E501


    :rtype: List[Attribute]
    """
    return 'do some magic!'


def get_data(attribute, simulation_steps=None, neuron_ids=None):  # noqa: E501
    """Retrieves the per-neuron attributes for the given attribute name, simulation steps (optional) and neuron IDs (optional).

     # noqa: E501

    :param attribute: Attribute name.
    :type attribute: str
    :param simulation_steps: Simulation steps (leave empty for all steps).
    :type simulation_steps: List[float]
    :param neuron_ids: Neuron IDs (leave empty for all neurons).
    :type neuron_ids: List[]

    :rtype: Data
    """
    return 'do some magic!'


def get_neuron_ids():  # noqa: E501
    """Retrieves the list of all neuron IDs.

     # noqa: E501


    :rtype: List[float]
    """
    return 'do some magic!'


def get_simulation_step_count():  # noqa: E501
    """Retrieves the number of simulation steps.

     # noqa: E501


    :rtype: float
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


def get_timestep():  # noqa: E501
    """Retrieves the time between two simulation steps.

     # noqa: E501


    :rtype: float
    """
    return 'do some magic!'
