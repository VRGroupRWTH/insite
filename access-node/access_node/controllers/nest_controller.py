import connexion
import six

from access_node.models.error_response import ErrorResponse  # noqa: E501
from access_node.models.multimeter_info import MultimeterInfo  # noqa: E501
from access_node.models.multimeter_measurement import MultimeterMeasurement  # noqa: E501
from access_node.models.nest_neuron_properties import NestNeuronProperties  # noqa: E501
from access_node.models.simulation_time_info import SimulationTimeInfo  # noqa: E501
from access_node.models.spikedetector_info import SpikedetectorInfo  # noqa: E501
from access_node.models.spikes import Spikes  # noqa: E501
from access_node import util


def nest_get_multimeter_by_id(multimeter_id):  # noqa: E501
    """Retreives the available multimeters and their properties.

     # noqa: E501

    :param multimeter_id: The identifier of the multimeter.
    :type multimeter_id: int

    :rtype: MultimeterInfo
    """
    return 'do some magic!'


def nest_get_multimeter_measurements(multimeter_id, attribute_name, from_time=None, to_time=None, neuron_ids=None, skip=None, top=None):  # noqa: E501
    """Retrieves the measurements for a multimeter, attribute and neuron IDs (optional).

     # noqa: E501

    :param multimeter_id: The multimeter to query
    :type multimeter_id: int
    :param attribute_name: The attribute to query (e.g., &#39;V_m&#39; for the membrane potential)
    :type attribute_name: str
    :param from_time: The start time (including) to be queried.
    :type from_time: float
    :param to_time: The end time (excluding) to be queried.
    :type to_time: float
    :param neuron_ids: A list of neuron IDs queried for attribute data.
    :type neuron_ids: List[int]
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


def nest_get_neuron_by_id(neuron_id):  # noqa: E501
    """Retrieves the properties of the specified neuron.

     # noqa: E501

    :param neuron_id: The ID of the queried neuron.
    :type neuron_id: int

    :rtype: NestNeuronProperties
    """
    return 'do some magic!'


def nest_get_neuron_ids():  # noqa: E501
    """Retrieves a list of all neuron IDs accessable via the pipeline.

     # noqa: E501


    :rtype: List[int]
    """
    return 'do some magic!'


def nest_get_neuron_ids_by_population(population_id):  # noqa: E501
    """Retrieves the list of all neuron ids within the population.

     # noqa: E501

    :param population_id: The identifier of the population
    :type population_id: int

    :rtype: List[int]
    """
    return 'do some magic!'


def nest_get_neurons():  # noqa: E501
    """Retrieves the properties of the specified neurons.

     # noqa: E501


    :rtype: List[NestNeuronProperties]
    """
    return 'do some magic!'


def nest_get_neurons_by_population(population_id):  # noqa: E501
    """Retrieves the neurons that belong to the specified population.

     # noqa: E501

    :param population_id: The population ID to query the neurons for.
    :type population_id: List[int]

    :rtype: List[NestNeuronProperties]
    """
    return 'do some magic!'


def nest_get_populations():  # noqa: E501
    """Retrieves a list of all accessable population IDs.

     # noqa: E501


    :rtype: List[int]
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


def nest_get_spikes(from_time=None, to_time=None, neuron_ids=None, skip=None, top=None):  # noqa: E501
    """Retrieves the spikes for the given time range (optional) and neuron IDs (optional). If no time range or neuron list is specified, it will return the spikes for whole time or all neurons respectively. This request merges the spikes recorded by all spike detectors and removes duplicates.

     # noqa: E501

    :param from_time: The start time in milliseconds (including) to be queried.
    :type from_time: float
    :param to_time: The end time in milliseconds (excluding) to be queried.
    :type to_time: float
    :param neuron_ids: A list of neuron IDs queried for spike data.
    :type neuron_ids: List[int]
    :param skip: The offset into the result.
    :type skip: int
    :param top: The maximum number of entries to be returned.
    :type top: int

    :rtype: Spikes
    """
    return 'do some magic!'


def nest_get_spikes_by_population(population_id, from_time=None, to_time=None, skip=None, top=None):  # noqa: E501
    """Retrieves the spikes for the given simulation steps (optional) and population. This request merges the spikes recorded by all spike detectors and removes duplicates.

     # noqa: E501

    :param population_id: The identifier of the population.
    :type population_id: int
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


def nest_get_spikes_by_spikedetector(spikedetector_id, from_time=None, to_time=None, neuron_ids=None, skip=None, top=None):  # noqa: E501
    """Retrieves the spikes for the given time range (optional) and neuron IDs (optional) from one spike detector. If no time range or neuron list is specified, it will return the spikes for whole time or all neurons respectively.

     # noqa: E501

    :param spikedetector_id: The ID of the spike detector to query.
    :type spikedetector_id: int
    :param from_time: The start time in milliseconds (including) to be queried.
    :type from_time: float
    :param to_time: The end time in milliseconds (excluding) to be queried.
    :type to_time: float
    :param neuron_ids: A list of neuron IDs queried for spike data.
    :type neuron_ids: List[int]
    :param skip: The offset into the result.
    :type skip: int
    :param top: The maximum number of entries to be returned.
    :type top: int

    :rtype: Spikes
    """
    return 'do some magic!'
