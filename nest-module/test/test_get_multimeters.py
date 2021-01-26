import requests
import numbers

def test_get_multimeters(nest_simulation):
    request = requests.get("http://localhost:9000/multimeters")
    assert(request.status_code == 200)
    
    multimeters = request.json()

    assert(isinstance(multimeters, list))

    for multimeter in multimeters:
        assert('multimeterId' in multimeter)
        assert(isinstance(multimeter['multimeterId'], int))

        assert('attributes' in multimeter)
        assert(isinstance(multimeter['attributes'], list))

        for attribute in multimeter['attributes']:
            assert(isinstance(attribute, str))

        assert('nodeIds' in multimeter)
        assert(isinstance(multimeter['nodeIds'], list))

        for node_id in multimeter['nodeIds']:
            assert(isinstance(node_id, int))
