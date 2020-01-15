import connexion
import six

from access_node.models.arbor_cell_properties import ArborCellProperties  # noqa: E501
from access_node.models.arbor_measurement import ArborMeasurement  # noqa: E501
from access_node.models.simulation_time_info import SimulationTimeInfo  # noqa: E501
from access_node.models.spikes import Spikes  # noqa: E501
from access_node import util


def arbor_get_cell_ids():  # noqa: E501
    """Retrieves the list of all cell ids.

     # noqa: E501


    :rtype: List[int]
    """
    return 'do some magic!'


def arbor_get_cell_properties(neuron_ids=None):  # noqa: E501
    """Retrieves the properties of the specified cells.

     # noqa: E501

    :param neuron_ids: A list of cell IDs queried for properties.
    :type neuron_ids: List[int]

    :rtype: List[ArborCellProperties]
    """
    return 'do some magic!'


def arbor_get_measurements(attribute, measurement_point_id=None, _from=None, to=None, offset=None, limit=None):  # noqa: E501
    """Retrieves the measurements for given measurement points (optional).

     # noqa: E501

    :param attribute: The attribute to query (e.g., &#39;V_m&#39; for the membrane potential)
    :type attribute: str
    :param measurement_point_id: A list of measurement point ids queried for data.
    :type measurement_point_id: List[int]
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
    return 'do some magic!'


def arbor_get_simulation_time_info():  # noqa: E501
    """Retrieves simulation time information.

     # noqa: E501


    :rtype: SimulationTimeInfo
    """
    return 'do some magic!'


def arbor_get_spikes(_from=None, to=None, gids=None, offset=None, limit=None):  # noqa: E501
    """Retrieves the spikes for the given simulation steps (optional) and cells (optional).

     # noqa: E501

    :param _from: The start time (including) to be queried.
    :type _from: float
    :param to: The end time (excluding) to be queried.
    :type to: float
    :param gids: A list of cells queried for spike data.
    :type gids: List[int]
    :param offset: The offset into the result.
    :type offset: int
    :param limit: The maximum of entries to be result.
    :type limit: int

    :rtype: Spikes
    """
    return 'do some magic!'
