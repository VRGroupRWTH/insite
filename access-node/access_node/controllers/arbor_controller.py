import connexion
import six

from access_node.models.arbor_cell_properties import ArborCellProperties  # noqa: E501
from access_node.models.arbor_measurement import ArborMeasurement  # noqa: E501
from access_node.models.probe import Probe  # noqa: E501
from access_node.models.simulation_time_info import SimulationTimeInfo  # noqa: E501
from access_node.models.spikes import Spikes  # noqa: E501
from access_node import util


def arbor_get_attributes():  # noqa: E501
    """Retrieves a list of all attributes.

     # noqa: E501


    :rtype: List[str]
    """
    return 'do some magic!'


def arbor_get_cell_ids():  # noqa: E501
    """Retrieves a list of all cell ids.

     # noqa: E501


    :rtype: List[int]
    """
    return 'do some magic!'


def arbor_get_cell_properties(cell_ids=None):  # noqa: E501
    """Retrieves the properties of the specified cells.

     # noqa: E501

    :param cell_ids: A list of cell IDs queried for properties.
    :type cell_ids: List[int]

    :rtype: List[ArborCellProperties]
    """
    return 'do some magic!'


def arbor_get_measurements(attribute, probe_ids=None, _from=None, to=None, offset=None, limit=None):  # noqa: E501
    """Retrieves the measurements for given probes (optional).

     # noqa: E501

    :param attribute: The attribute to query
    :type attribute: str
    :param probe_ids: A list of probes ids queried for data.
    :type probe_ids: List[int]
    :param _from: The start time (including) to be queried.
    :type _from: float
    :param to: The end time (excluding) to be queried.
    :type to: float
    :param offset: The offset into the result.
    :type offset: int
    :param limit: The maximum number of entries to be returned.
    :type limit: int

    :rtype: ArborMeasurement
    """
    return 'do some magic!'


def arbor_get_probes(attribute=None):  # noqa: E501
    """Retrieves a list of all probes for a given attribute (optional).

     # noqa: E501

    :param attribute: The attribute measured for which existing probes will be returned.
    :type attribute: str

    :rtype: List[Probe]
    """
    return 'do some magic!'


def arbor_get_simulation_time_info():  # noqa: E501
    """Retrieves simulation time information(begin, current, end).

     # noqa: E501


    :rtype: SimulationTimeInfo
    """
    return 'do some magic!'


def arbor_get_spikes(_from=None, to=None, cell_ids=None, segment_ids=None, offset=None, limit=None):  # noqa: E501
    """Retrieves the spikes for the given simulation times (optional), cell and segment (optional).

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
    :param limit: The maximum number of entries to be result.
    :type limit: int

    :rtype: Spikes
    """
    return 'do some magic!'
