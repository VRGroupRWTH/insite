# coding: utf-8

from __future__ import absolute_import
from datetime import date, datetime  # noqa: F401

from typing import List, Dict  # noqa: F401

from access_node.models.base_model_ import Model
from access_node import util


class ArborCellProperties(Model):
    """NOTE: This class is auto generated by OpenAPI Generator (https://openapi-generator.tech).

    Do not edit the class manually.
    """

    def __init__(self, neuron_id=None, properties=None):  # noqa: E501
        """ArborCellProperties - a model defined in OpenAPI

        :param neuron_id: The neuron_id of this ArborCellProperties.  # noqa: E501
        :type neuron_id: int
        :param properties: The properties of this ArborCellProperties.  # noqa: E501
        :type properties: object
        """
        self.openapi_types = {
            'neuron_id': int,
            'properties': object
        }

        self.attribute_map = {
            'neuron_id': 'neuron_id',
            'properties': 'properties'
        }

        self._neuron_id = neuron_id
        self._properties = properties

    @classmethod
    def from_dict(cls, dikt) -> 'ArborCellProperties':
        """Returns the dict as a model

        :param dikt: A dict.
        :type: dict
        :return: The ArborCellProperties of this ArborCellProperties.  # noqa: E501
        :rtype: ArborCellProperties
        """
        return util.deserialize_model(dikt, cls)

    @property
    def neuron_id(self):
        """Gets the neuron_id of this ArborCellProperties.


        :return: The neuron_id of this ArborCellProperties.
        :rtype: int
        """
        return self._neuron_id

    @neuron_id.setter
    def neuron_id(self, neuron_id):
        """Sets the neuron_id of this ArborCellProperties.


        :param neuron_id: The neuron_id of this ArborCellProperties.
        :type neuron_id: int
        """

        self._neuron_id = neuron_id

    @property
    def properties(self):
        """Gets the properties of this ArborCellProperties.


        :return: The properties of this ArborCellProperties.
        :rtype: object
        """
        return self._properties

    @properties.setter
    def properties(self, properties):
        """Sets the properties of this ArborCellProperties.


        :param properties: The properties of this ArborCellProperties.
        :type properties: object
        """

        self._properties = properties
