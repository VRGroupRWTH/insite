import requests
import numbers


def test_get_nest_node_collections(nest_simulation):
    r = requests.get("http://localhost:8080/nest/nodeCollections")
    assert(r.status_code == 200)

    node_collections = r.json()

    for collection in node_collections:
      assert(isinstance(collection, dict))
      
      assert('model' in collection)
      assert('name' in collection["model"])
      assert('status' in collection["model"])

      assert('nodes' in collection)
      assert('count' in collection["nodes"])
      assert('firstId' in collection["nodes"])
      assert('lastId' in collection["nodes"])

      assert('nodeCollectionId' in collection)


      
    