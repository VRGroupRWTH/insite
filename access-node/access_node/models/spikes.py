# coding: utf-8

from __future__ import absolute_import
from datetime import date, datetime  # noqa: F401

from typing import List, Dict  # noqa: F401

from access_node.models.base_model_ import Model
from access_node import util


class Spikes(Model):
    """NOTE: This class is auto generated by OpenAPI Generator (https://openapi-generator.tech).

    Do not edit the class manually.
    """

    def __init__(self, simulation_times=None, node_ids=None):  # noqa: E501
        """Spikes - a model defined in OpenAPI

        :param simulation_times: The simulation_times of this Spikes.  # noqa: E501
        :type simulation_times: List[float]
        :param node_ids: The node_ids of this Spikes.  # noqa: E501
        :type node_ids: List[int]
        """
        self.openapi_types = {
            'simulation_times': List[float],
            'node_ids': List[int]
        }

        self.attribute_map = {
            'simulation_times': 'simulationTimes',
            'node_ids': 'nodeIds'
        }

        self._simulation_times = simulation_times
        self._node_ids = node_ids

    @classmethod
    def from_dict(cls, dikt) -> 'Spikes':
        """Returns the dict as a model

        :param dikt: A dict.
        :type: dict
        :return: The Spikes of this Spikes.  # noqa: E501
        :rtype: Spikes
        """
        return util.deserialize_model(dikt, cls)

    @property
    def simulation_times(self):
        """Gets the simulation_times of this Spikes.

        This array is always sorted.  # noqa: E501

        :return: The simulation_times of this Spikes.
        :rtype: List[float]
        """
        return self._simulation_times

    @simulation_times.setter
    def simulation_times(self, simulation_times):
        """Sets the simulation_times of this Spikes.

        This array is always sorted.  # noqa: E501

        :param simulation_times: The simulation_times of this Spikes.
        :type simulation_times: List[float]
        """

        self._simulation_times = simulation_times

    @property
    def node_ids(self):
        """Gets the node_ids of this Spikes.


        :return: The node_ids of this Spikes.
        :rtype: List[int]
        """
        return self._node_ids

    @node_ids.setter
    def node_ids(self, node_ids):
        """Sets the node_ids of this Spikes.


        :param node_ids: The node_ids of this Spikes.
        :type node_ids: List[int]
        """

        self._node_ids = node_ids
