# coding: utf-8

from __future__ import absolute_import

from flask import json
from six import BytesIO

from access_node.models.arbor_measurement import ArborMeasurement  # noqa: E501
from access_node.models.arbor_neuron_properties import ArborNeuronProperties  # noqa: E501
from access_node.models.simulation_time_info import SimulationTimeInfo  # noqa: E501
from access_node.models.spikes import Spikes  # noqa: E501
from access_node.test import BaseTestCase


class TestArborController(BaseTestCase):
    """ArborController integration test stubs"""

    def test_arbor_get_attributes(self):
        """Test case for arbor_get_attributes

        Retrieves a list of measurable attributes
        """
        response = self.client.open(
            '//arbor/attributes',
            method='GET')
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))

    def test_arbor_get_measurements(self):
        """Test case for arbor_get_measurements

        Retrieves the measurements for given attribute and neuron_ids (optional).
        """
        query_string = [('attribute', 'attribute_example'),
                        ('_from', 1.2),
                        ('to', 1.2),
                        ('neuron_ids', 56),
                        ('offset', 56),
                        ('limit', 56)]
        response = self.client.open(
            '//arbor/measurements',
            method='GET',
            content_type='application/json',
            query_string=query_string)
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))

    def test_arbor_get_neuron_ids(self):
        """Test case for arbor_get_neuron_ids

        Retrieves the list of all neuron ids.
        """
        response = self.client.open(
            '//arbor/neuron_ids',
            method='GET')
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))

    def test_arbor_get_neuron_properties(self):
        """Test case for arbor_get_neuron_properties

        Retrieves the properties of the specified neurons.
        """
        query_string = [('neuron_ids', 56)]
        response = self.client.open(
            '//arbor/neuron_properties',
            method='GET',
            query_string=query_string)
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))

    def test_arbor_get_simulation_time_info(self):
        """Test case for arbor_get_simulation_time_info

        Retrieves simulation time information.
        """
        response = self.client.open(
            '//arbor/simulation_time_info',
            method='GET')
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))

    def test_arbor_get_spikes(self):
        """Test case for arbor_get_spikes

        Retrieves the spikes for the given simulation steps (optional) and GIDS (optional).
        """
        query_string = [('_from', 1.2),
                        ('to', 1.2),
                        ('gids', 56),
                        ('offset', 56),
                        ('limit', 56)]
        response = self.client.open(
            '//arbor/spikes',
            method='GET',
            content_type='application/json',
            query_string=query_string)
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))


if __name__ == '__main__':
    import unittest
    unittest.main()
