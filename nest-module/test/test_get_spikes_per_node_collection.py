import requests
import numbers


def test_get_spikes_per_node_collection(nest_simulation):
    r = requests.get("http://localhost:8080/nest/nodeCollections")
    assert(r.status_code == 200)

    node_collections = r.json()
    collection_ids = []

    for collection in node_collections:
      collection_ids.append(collection["nodeCollectionId"])
    
    r = requests.get("http://localhost:8080/nest/nodeCollections")
    assert(r.status_code == 200)
    node_collections = r.json()


    for collection_id in collection_ids:
      url = "http://localhost:8080/nest/nodeCollections/{0}/spikes".format(collection_id)
      r = requests.get(url)
      assert(r.status_code == 200)
      spikes = r.json()

      current_collection = next(item for item in node_collections if item['nodeCollectionId'] == collection_id)

      for node_id in spikes['nodeIds']:
        assert(node_id <= current_collection['nodes']['lastId'])
        assert(node_id >= current_collection['nodes']['firstId'])

     
  