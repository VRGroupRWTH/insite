# coding: utf-8

from __future__ import absolute_import
import unittest

from flask import json
from six import BytesIO

from access_node.models.arbor_cell_properties import ArborCellProperties  # noqa: E501
from access_node.models.arbor_measurement import ArborMeasurement  # noqa: E501
from access_node.models.probe import Probe  # noqa: E501
from access_node.models.simulation_time_info import SimulationTimeInfo  # noqa: E501
from access_node.models.spikes import Spikes  # noqa: E501
from access_node.test import BaseTestCase


class TestArborController(BaseTestCase):
    """ArborController integration test stubs"""

    def test_arbor_get_attributes(self):
        """Test case for arbor_get_attributes

        Retrieves a list of all attributes.
        """
        headers = { 
            'Accept': 'application/json',
        }
        response = self.client.open(
            '/v1/arbor/attributes',
            method='GET',
            headers=headers)
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))

    def test_arbor_get_cell_ids(self):
        """Test case for arbor_get_cell_ids

        Retrieves a list of all cell ids.
        """
        headers = { 
            'Accept': 'application/json',
        }
        response = self.client.open(
            '/v1/arbor/cell_ids',
            method='GET',
            headers=headers)
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))

    def test_arbor_get_cell_properties(self):
        """Test case for arbor_get_cell_properties

        Retrieves the properties of the specified cells.
        """
        query_string = [('cell_ids', 56)]
        headers = { 
            'Accept': 'application/json',
        }
        response = self.client.open(
            '/v1/arbor/cell_properties',
            method='GET',
            headers=headers,
            query_string=query_string)
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))

    def test_arbor_get_measurements(self):
        """Test case for arbor_get_measurements

        Retrieves the measurements for given probes (optional).
        """
        query_string = [('attribute', 'attribute_example'),
                        ('probe_ids', 56),
                        ('from', 3.4),
                        ('to', 3.4),
                        ('offset', 56),
                        ('limit', 56)]
        headers = { 
            'Accept': 'application/json',
        }
        response = self.client.open(
            '/v1/arbor/measurements',
            method='GET',
            headers=headers,
            query_string=query_string)
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))

    def test_arbor_get_probes(self):
        """Test case for arbor_get_probes

        Retrieves a list of all probes for a given attribute (optional).
        """
        query_string = [('attribute', 'attribute_example')]
        headers = { 
            'Accept': 'application/json',
        }
        response = self.client.open(
            '/v1/arbor/probes',
            method='GET',
            headers=headers,
            query_string=query_string)
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))

    def test_arbor_get_simulation_time_info(self):
        """Test case for arbor_get_simulation_time_info

        Retrieves simulation time information(begin, current, end).
        """
        headers = { 
            'Accept': 'application/json',
        }
        response = self.client.open(
            '/v1/arbor/simulation_time_info',
            method='GET',
            headers=headers)
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))

    def test_arbor_get_spikes(self):
        """Test case for arbor_get_spikes

        Retrieves the spikes for the given simulation times (optional), cell and segment (optional).
        """
        query_string = [('from', 3.4),
                        ('to', 3.4),
                        ('cell_ids', 56),
                        ('segment_ids', 56),
                        ('offset', 56),
                        ('limit', 56)]
        headers = { 
            'Accept': 'application/json',
        }
        response = self.client.open(
            '/v1/arbor/spikes',
            method='GET',
            headers=headers,
            query_string=query_string)
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))


if __name__ == '__main__':
    unittest.main()
