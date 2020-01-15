#!/usr/bin/env python3

import connexion

from access_node import encoder
from flask_cors import CORS

from access_node.models.nodes import nodes
import json

import requests
import time


def main():
    # This is just to give the info-node some time to start the server
    # in the docker container
    time.sleep(5)

    # get info node
    with open('access_node//info_node.json', 'r') as f:
        info = json.load(f)
    nodes.info_node = info['address']

    # get simulation nodes
    node_type = 'nest_simulation'
    nodes.nest_simulation_nodes = requests.get(
        nodes.info_node+'/nodes', params={"node_type": node_type}).json()
    node_type = 'arbor_simulation'
    nodes.arbor_simulation_nodes = requests.get(
        nodes.info_node+'/nodes', params={"node_type": node_type}).json()

    # run acces_node
    app = connexion.App(__name__, specification_dir='./swagger/')
    app.app.json_encoder = encoder.JSONEncoder
    app.add_api('swagger.yaml', arguments={
                'title': 'In-Situ Pipeline REST API'})
    CORS(app.app)
    app.run(port=8080)


if __name__ == '__main__':
    main()
