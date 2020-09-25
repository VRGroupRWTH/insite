# coding: utf-8

from __future__ import absolute_import
import unittest

from flask import json
from six import BytesIO

from access_node.models.error_response import ErrorResponse  # noqa: E501
from access_node.models.multimeter_info import MultimeterInfo  # noqa: E501
from access_node.models.multimeter_measurement import MultimeterMeasurement  # noqa: E501
from access_node.models.nest_neuron_properties import NestNeuronProperties  # noqa: E501
from access_node.models.simulation_time_info import SimulationTimeInfo  # noqa: E501
from access_node.models.spikedetector_info import SpikedetectorInfo  # noqa: E501
from access_node.models.spikes import Spikes  # noqa: E501
from access_node.test import BaseTestCase


class TestNestController(BaseTestCase):
    """NestController integration test stubs"""

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

        Retrieves the measurements for a multimeter, attribute and neuron IDs (optional).
        """
        query_string = [('fromTime', 3.4),
                        ('toTime', 3.4),
                        ('neuronIds', 56),
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

    def test_nest_get_neuron_by_id(self):
        """Test case for nest_get_neuron_by_id

        Retrieves the properties of the specified neuron.
        """
        headers = { 
            'Accept': 'application/json',
        }
        response = self.client.open(
            '/v1/nest/neurons/{neuron_id}'.format(neuron_id=56),
            method='GET',
            headers=headers)
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))

    def test_nest_get_neuron_ids(self):
        """Test case for nest_get_neuron_ids

        Retrieves a list of all neuron IDs accessable via the pipeline.
        """
        headers = { 
            'Accept': 'application/json',
        }
        response = self.client.open(
            '/v1/nest/neurons/ids',
            method='GET',
            headers=headers)
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))

    def test_nest_get_neuron_ids_by_population(self):
        """Test case for nest_get_neuron_ids_by_population

        Retrieves the list of all neuron ids within the population.
        """
        headers = { 
            'Accept': 'application/json',
        }
        response = self.client.open(
            '/v1/nest/populations/${populationId}/neurons/ids'.format(population_id=56),
            method='GET',
            headers=headers)
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))

    def test_nest_get_neurons(self):
        """Test case for nest_get_neurons

        Retrieves the properties of the specified neurons.
        """
        headers = { 
            'Accept': 'application/json',
        }
        response = self.client.open(
            '/v1/nest/neurons',
            method='GET',
            headers=headers)
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))

    def test_nest_get_neurons_by_population(self):
        """Test case for nest_get_neurons_by_population

        Retrieves the neurons that belong to the specified population.
        """
        headers = { 
            'Accept': 'application/json',
        }
        response = self.client.open(
            '/v1/nest/populations/{population_id}/neurons'.format(population_id=56),
            method='GET',
            headers=headers)
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))

    def test_nest_get_populations(self):
        """Test case for nest_get_populations

        Retrieves a list of all accessable population IDs.
        """
        headers = { 
            'Accept': 'application/json',
        }
        response = self.client.open(
            '/v1/nest/populations',
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

        Retrieves the spikes for the given time range (optional) and neuron IDs (optional). If no time range or neuron list is specified, it will return the spikes for whole time or all neurons respectively. This request merges the spikes recorded by all spike detectors and removes duplicates.
        """
        query_string = [('fromTime', 3.4),
                        ('toTime', 3.4),
                        ('neuronIds', 56),
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

    def test_nest_get_spikes_by_population(self):
        """Test case for nest_get_spikes_by_population

        Retrieves the spikes for the given simulation steps (optional) and population. This request merges the spikes recorded by all spike detectors and removes duplicates.
        """
        query_string = [('fromTime', 3.4),
                        ('toTime', 3.4),
                        ('skip', 56),
                        ('top', 56)]
        headers = { 
            'Accept': 'application/json',
        }
        response = self.client.open(
            '/v1/nest/populations/{population_id}/spikes'.format(population_id=56),
            method='GET',
            headers=headers,
            query_string=query_string)
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))

    def test_nest_get_spikes_by_spikedetector(self):
        """Test case for nest_get_spikes_by_spikedetector

        Retrieves the spikes for the given time range (optional) and neuron IDs (optional) from one spike detector. If no time range or neuron list is specified, it will return the spikes for whole time or all neurons respectively.
        """
        query_string = [('fromTime', 3.4),
                        ('toTime', 3.4),
                        ('neuronIds', 56),
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
