# coding: utf-8

from __future__ import absolute_import
from datetime import date, datetime  # noqa: F401

from typing import List, Dict  # noqa: F401

from access_node.models.base_model_ import Model
from access_node.models.nest_node_model import NestNodeModel
from access_node import util

from access_node.models.nest_node_model import NestNodeModel  # noqa: E501

class NestNodeProperties(Model):
    """NOTE: This class is auto generated by OpenAPI Generator (https://openapi-generator.tech).

    Do not edit the class manually.
    """

    def __init__(self, node_id=None, node_collection_id=None, model=None, position=None):  # noqa: E501
        """NestNodeProperties - a model defined in OpenAPI

        :param node_id: The node_id of this NestNodeProperties.  # noqa: E501
        :type node_id: int
        :param node_collection_id: The node_collection_id of this NestNodeProperties.  # noqa: E501
        :type node_collection_id: int
        :param model: The model of this NestNodeProperties.  # noqa: E501
        :type model: NestNodeModel
        :param position: The position of this NestNodeProperties.  # noqa: E501
        :type position: List[float]
        """
        self.openapi_types = {
            'node_id': int,
            'node_collection_id': int,
            'model': NestNodeModel,
            'position': List[float]
        }

        self.attribute_map = {
            'node_id': 'nodeId',
            'node_collection_id': 'nodeCollectionId',
            'model': 'model',
            'position': 'position'
        }

        self._node_id = node_id
        self._node_collection_id = node_collection_id
        self._model = model
        self._position = position

    @classmethod
    def from_dict(cls, dikt) -> 'NestNodeProperties':
        """Returns the dict as a model

        :param dikt: A dict.
        :type: dict
        :return: The NestNodeProperties of this NestNodeProperties.  # noqa: E501
        :rtype: NestNodeProperties
        """
        return util.deserialize_model(dikt, cls)

    @property
    def node_id(self):
        """Gets the node_id of this NestNodeProperties.


        :return: The node_id of this NestNodeProperties.
        :rtype: int
        """
        return self._node_id

    @node_id.setter
    def node_id(self, node_id):
        """Sets the node_id of this NestNodeProperties.


        :param node_id: The node_id of this NestNodeProperties.
        :type node_id: int
        """

        self._node_id = node_id

    @property
    def node_collection_id(self):
        """Gets the node_collection_id of this NestNodeProperties.


        :return: The node_collection_id of this NestNodeProperties.
        :rtype: int
        """
        return self._node_collection_id

    @node_collection_id.setter
    def node_collection_id(self, node_collection_id):
        """Sets the node_collection_id of this NestNodeProperties.


        :param node_collection_id: The node_collection_id of this NestNodeProperties.
        :type node_collection_id: int
        """

        self._node_collection_id = node_collection_id

    @property
    def model(self):
        """Gets the model of this NestNodeProperties.


        :return: The model of this NestNodeProperties.
        :rtype: NestNodeModel
        """
        return self._model

    @model.setter
    def model(self, model):
        """Sets the model of this NestNodeProperties.


        :param model: The model of this NestNodeProperties.
        :type model: NestNodeModel
        """

        self._model = model

    @property
    def position(self):
        """Gets the position of this NestNodeProperties.


        :return: The position of this NestNodeProperties.
        :rtype: List[float]
        """
        return self._position

    @position.setter
    def position(self, position):
        """Sets the position of this NestNodeProperties.


        :param position: The position of this NestNodeProperties.
        :type position: List[float]
        """

        self._position = position