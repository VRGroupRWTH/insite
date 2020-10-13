# coding: utf-8

from __future__ import absolute_import
import unittest

from flask import json
from six import BytesIO

from access_node.models.error_response import ErrorResponse  # noqa: E501
from access_node.models.multimeter_info import MultimeterInfo  # noqa: E501
from access_node.models.multimeter_measurement import MultimeterMeasurement  # noqa: E501
from access_node.models.nest_node_collection_properties import NestNodeCollectionProperties  # noqa: E501
from access_node.models.nest_node_properties import NestNodeProperties  # noqa: E501
from access_node.models.simulation_time_info import SimulationTimeInfo  # noqa: E501
from access_node.models.spikedetector_info import SpikedetectorInfo  # noqa: E501
from access_node.models.spikes import Spikes  # noqa: E501
from access_node.test import BaseTestCase


class TestNestController(BaseTestCase):
    """NestController integration test stubs"""

    def test_nest_get_kernel_status(self):
        """Test case for nest_get_kernel_status

        Retreives the current status of the NEST kernel.
        """
        headers = { 
            'Accept': 'application/json',
        }
        response = self.client.open(
            '/v1/nest/kernelStatus',
            method='GET',
            headers=headers)
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))

    def test_nest_get_multimeter_by_id(self):
        """Test case for nest_get_multimeter_by_id

        Retreives the available multimeters and their properties.
        """
        headers = { 
            'Accept': 'application/json',
        }
        response = self.client.open(
            '/v1/nest/multimeters/{multimeter_id}'.format(multimeter_id=56),
            method='GET',
            headers=headers)
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))

    def test_nest_get_multimeter_measurements(self):
        """Test case for nest_get_multimeter_measurements

        Retrieves the measurements for a multimeter, attribute and node IDs (optional).
        """
        query_string = [('fromTime', 3.4),
                        ('toTime', 3.4),
                        ('nodeIds', 56),
                        ('skip', 56),
                        ('top', 56)]
        headers = { 
            'Accept': 'application/json',
        }
        response = self.client.open(
            '/v1/nest/multimeters/{multimeter_id}/attributes/{attribute_name}'.format(multimeter_id=56, attribute_name='attribute_name_example'),
            method='GET',
            headers=headers,
            query_string=query_string)
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))

    def test_nest_get_multimeters(self):
        """Test case for nest_get_multimeters

        Retreives the available multimeters and their properties.
        """
        headers = { 
            'Accept': 'application/json',
        }
        response = self.client.open(
            '/v1/nest/multimeters',
            method='GET',
            headers=headers)
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))

    def test_nest_get_node_by_id(self):
        """Test case for nest_get_node_by_id

        Retrieves the properties of the specified node.
        """
        headers = { 
            'Accept': 'application/json',
        }
        response = self.client.open(
            '/v1/nest/nodes/{node_id}'.format(node_id=56),
            method='GET',
            headers=headers)
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))

    def test_nest_get_node_collections(self):
        """Test case for nest_get_node_collections

        Retrieves a list of all node collection IDs.
        """
        headers = { 
            'Accept': 'application/json',
        }
        response = self.client.open(
            '/v1/nest/nodeCollections',
            method='GET',
            headers=headers)
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))

    def test_nest_get_node_ids(self):
        """Test case for nest_get_node_ids

        Retrieves a list of all node IDs.
        """
        headers = { 
            'Accept': 'application/json',
        }
        response = self.client.open(
            '/v1/nest/nodes/ids',
            method='GET',
            headers=headers)
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))

    def test_nest_get_node_ids_by_node_collection(self):
        """Test case for nest_get_node_ids_by_node_collection

        Retrieves the list of all node ids within the node collection.
        """
        headers = { 
            'Accept': 'application/json',
        }
        response = self.client.open(
            '/v1/nest/nodeCollections/${nodeCollectionId}/nodes/ids'.format(node_collection_id=56),
            method='GET',
            headers=headers)
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))

    def test_nest_get_nodes(self):
        """Test case for nest_get_nodes

        Retrieves all nodes of the simulation.
        """
        headers = { 
            'Accept': 'application/json',
        }
        response = self.client.open(
            '/v1/nest/nodes',
            method='GET',
            headers=headers)
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))

    def test_nest_get_nodes_by_node_collection(self):
        """Test case for nest_get_nodes_by_node_collection

        Retrieves the list of all node within the node collection.
        """
        headers = { 
            'Accept': 'application/json',
        }
        response = self.client.open(
            '/v1/nest/nodeCollections/${nodeCollectionId}/nodes'.format(node_collection_id=56),
            method='GET',
            headers=headers)
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))

    def test_nest_get_simulation_time_info(self):
        """Test case for nest_get_simulation_time_info

        Retrieves simulation time information (stepSize, begin, current, end).
        """
        headers = { 
            'Accept': 'application/json',
        }
        response = self.client.open(
            '/v1/nest/simulationTimeInfo',
            method='GET',
            headers=headers)
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))

    def test_nest_get_spikedetectors(self):
        """Test case for nest_get_spikedetectors

        Queries all spike detectors accessable to the pipeline.
        """
        headers = { 
            'Accept': 'application/json',
        }
        response = self.client.open(
            '/v1/nest/spikedetectors/',
            method='GET',
            headers=headers)
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))

    def test_nest_get_spikes(self):
        """Test case for nest_get_spikes

        Retrieves the spikes for the given time range (optional) and node IDs (optional). If no time range or node list is specified, it will return the spikes for whole time or all nodes respectively. This request merges the spikes recorded by all spike detectors and removes duplicates.
        """
        query_string = [('fromTime', 3.4),
                        ('toTime', 3.4),
                        ('nodeIds', 56),
                        ('skip', 56),
                        ('top', 56)]
        headers = { 
            'Accept': 'application/json',
        }
        response = self.client.open(
            '/v1/nest/spikes',
            method='GET',
            headers=headers,
            query_string=query_string)
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))

    def test_nest_get_spikes_by_node_collection(self):
        """Test case for nest_get_spikes_by_node_collection

        Retrieves the spikes for the given simulation steps (optional) and node collection. This request merges the spikes recorded by all spike detectors and removes duplicates.
        """
        query_string = [('fromTime', 3.4),
                        ('toTime', 3.4),
                        ('skip', 56),
                        ('top', 56)]
        headers = { 
            'Accept': 'application/json',
        }
        response = self.client.open(
            '/v1/nest/nodeCollections/{node_collection_id}/spikes'.format(node_collection_id=56),
            method='GET',
            headers=headers,
            query_string=query_string)
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))

    def test_nest_get_spikes_by_spikedetector(self):
        """Test case for nest_get_spikes_by_spikedetector

        Retrieves the spikes for the given time range (optional) and node IDs (optional) from one spike detector. If no time range or node list is specified, it will return the spikes for whole time or all nodes respectively.
        """
        query_string = [('fromTime', 3.4),
                        ('toTime', 3.4),
                        ('nodeIds', 56),
                        ('skip', 56),
                        ('top', 56)]
        headers = { 
            'Accept': 'application/json',
        }
        response = self.client.open(
            '/v1/nest/spikedetectors/{spikedetector_id}/spikes'.format(spikedetector_id=56),
            method='GET',
            headers=headers,
            query_string=query_string)
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))


if __name__ == '__main__':
    unittest.main()
