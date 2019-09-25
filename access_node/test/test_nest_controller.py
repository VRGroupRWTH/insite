# coding: utf-8

from __future__ import absolute_import

from flask import json
from six import BytesIO

from access_node.models.attribute import Attribute  # noqa: E501
from access_node.models.data import Data  # noqa: E501
from access_node.models.error import Error  # noqa: E501
from access_node.models.spikes import Spikes  # noqa: E501
from access_node.test import BaseTestCase


class TestNestController(BaseTestCase):
    """NestController integration test stubs"""

    def test_get_attributes(self):
        """Test case for get_attributes

        Retrieves the details of per-neuron attributes.
        """
        response = self.client.open(
            '//attributes',
            method='GET')
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))

    def test_get_data(self):
        """Test case for get_data

        Retrieves the per-neuron attributes for the given attribute name, simulation steps (optional) and neuron IDs (optional).
        """
        query_string = [('attribute', 'attribute_example'),
                        ('simulation_steps', 3.4),
                        ('neuron_ids', 3.4)]
        response = self.client.open(
            '//data',
            method='GET',
            content_type='application/json',
            query_string=query_string)
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))

    def test_get_neuron_ids(self):
        """Test case for get_neuron_ids

        Retrieves the list of all neuron IDs.
        """
        response = self.client.open(
            '//neuron_ids',
            method='GET')
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))

    def test_get_simulation_step_count(self):
        """Test case for get_simulation_step_count

        Retrieves the number of simulation steps.
        """
        response = self.client.open(
            '//simulation_step_count',
            method='GET')
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))

    def test_get_spikes(self):
        """Test case for get_spikes

        Retrieves the spikes for the given simulation steps (optional) and neuron IDs (optional).
        """
        query_string = [('simulation_steps', 3.4),
                        ('neuron_ids', 3.4)]
        response = self.client.open(
            '//spikes',
            method='GET',
            content_type='application/json',
            query_string=query_string)
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))

    def test_get_timestep(self):
        """Test case for get_timestep

        Retrieves the time between two simulation steps.
        """
        response = self.client.open(
            '//timestep',
            method='GET')
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))


if __name__ == '__main__':
    import unittest
    unittest.main()
