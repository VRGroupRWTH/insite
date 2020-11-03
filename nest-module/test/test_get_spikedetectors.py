import requests
import numbers


def test_get_spikedetectors(nest_simulation):
    r = requests.get("http://localhost:8080/nest/spikedetectors")
    assert(r.status_code == 200)

    spikedetectors = r.json()

    for spikedetector in spikedetectors:
      assert(isinstance(spikedetector, dict))
      
      assert('nodeIds' in spikedetector)
      assert('spikedetectorId' in spikedetector)
    