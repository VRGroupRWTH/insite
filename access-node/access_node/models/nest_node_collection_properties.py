# coding: utf-8

from __future__ import absolute_import
from datetime import date, datetime  # noqa: F401

from typing import List, Dict  # noqa: F401

from access_node.models.base_model_ import Model
from access_node.models.nest_node_collection_nodes import NestNodeCollectionNodes
from access_node.models.nest_node_model import NestNodeModel
from access_node import util

from access_node.models.nest_node_collection_nodes import NestNodeCollectionNodes  # noqa: E501
from access_node.models.nest_node_model import NestNodeModel  # noqa: E501

class NestNodeCollectionProperties(Model):
    """NOTE: This class is auto generated by OpenAPI Generator (https://openapi-generator.tech).

    Do not edit the class manually.
    """

    def __init__(self, node_collection_id=None, nodes=None, model=None):  # noqa: E501
        """NestNodeCollectionProperties - a model defined in OpenAPI

        :param node_collection_id: The node_collection_id of this NestNodeCollectionProperties.  # noqa: E501
        :type node_collection_id: int
        :param nodes: The nodes of this NestNodeCollectionProperties.  # noqa: E501
        :type nodes: NestNodeCollectionNodes
        :param model: The model of this NestNodeCollectionProperties.  # noqa: E501
        :type model: NestNodeModel
        """
        self.openapi_types = {
            'node_collection_id': int,
            'nodes': NestNodeCollectionNodes,
            'model': NestNodeModel
        }

        self.attribute_map = {
            'node_collection_id': 'nodeCollectionId',
            'nodes': 'nodes',
            'model': 'model'
        }

        self._node_collection_id = node_collection_id
        self._nodes = nodes
        self._model = model

    @classmethod
    def from_dict(cls, dikt) -> 'NestNodeCollectionProperties':
        """Returns the dict as a model

        :param dikt: A dict.
        :type: dict
        :return: The NestNodeCollectionProperties of this NestNodeCollectionProperties.  # noqa: E501
        :rtype: NestNodeCollectionProperties
        """
        return util.deserialize_model(dikt, cls)

    @property
    def node_collection_id(self):
        """Gets the node_collection_id of this NestNodeCollectionProperties.


        :return: The node_collection_id of this NestNodeCollectionProperties.
        :rtype: int
        """
        return self._node_collection_id

    @node_collection_id.setter
    def node_collection_id(self, node_collection_id):
        """Sets the node_collection_id of this NestNodeCollectionProperties.


        :param node_collection_id: The node_collection_id of this NestNodeCollectionProperties.
        :type node_collection_id: int
        """

        self._node_collection_id = node_collection_id

    @property
    def nodes(self):
        """Gets the nodes of this NestNodeCollectionProperties.


        :return: The nodes of this NestNodeCollectionProperties.
        :rtype: NestNodeCollectionNodes
        """
        return self._nodes

    @nodes.setter
    def nodes(self, nodes):
        """Sets the nodes of this NestNodeCollectionProperties.


        :param nodes: The nodes of this NestNodeCollectionProperties.
        :type nodes: NestNodeCollectionNodes
        """

        self._nodes = nodes

    @property
    def model(self):
        """Gets the model of this NestNodeCollectionProperties.


        :return: The model of this NestNodeCollectionProperties.
        :rtype: NestNodeModel
        """
        return self._model

    @model.setter
    def model(self, model):
        """Sets the model of this NestNodeCollectionProperties.


        :param model: The model of this NestNodeCollectionProperties.
        :type model: NestNodeModel
        """

        self._model = model