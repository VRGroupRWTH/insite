import requests
import numbers

# Currently, this endpoint does not behave according to the specs, uncomment this test if they do

# def test_get_neuron_properties(nest_simulation):
#     r = requests.get("http://localhost:8080/nest/neuron_properties")
#     neurons_properties = r.json()

#     assert(isinstance(neurons_properties, list))

#     for neuron_properties in neurons_properties:
#         assert('gid' in neuron_properties)
#         assert(isinstance(neuron_properties['gid'], numbers.Number))
#         assert('properties' in neuron_properties)
#         assert('population' in neuron_properties['properties'])
#         assert(isinstance(neuron_properties['properties']['population'], numbers.Integral))
#         assert('position' in neuron_properties['properties'])
#         assert(isinstance(neuron_properties['properties']['position'], list))

# def test_get_neuron_properties_with_gid_filter(nest_simulation):
#     r = requests.get("http://localhost:8080/nest/neuron_properties?gids=3&gids=2")
#     neurons_properties = r.json()

#     assert(isinstance(neurons_properties, list))
#     assert(len(neurons_properties) == 2)

#     for neuron_properties in neurons_properties:
#         assert('gid' in neuron_properties)
#         assert(isinstance(neuron_properties['gid'], numbers.Number))
#         assert('properties' in neuron_properties)
#         assert('population' in neuron_properties['properties'])
#         assert(isinstance(neuron_properties['properties']['population'], numbers.Integral))
#         assert('position' in neuron_properties['properties'])
#         assert(isinstance(neuron_properties['properties']['position'], list))

#     assert(neuron_properties[0].gid == 3)
#     assert(neuron_properties[1].gid == 2)


