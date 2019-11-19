#!/usr/bin/env python3

import connexion

from access_node import encoder

from access_node.models.nodes import simulation_nodes, info_node
import json

import requests


def main():
    # get simulation nodes
    with open('access_node//info_node.json', 'r') as f:
        info = json.load(f)
    info_node = info['address']

    node_type = 'nest_simulation'
    simulation_nodes = requests.get(info_node+'/nodes', params={"node_type": node_type }).json()

    # run acces_node
    app = connexion.App(__name__, specification_dir='./swagger/')
    app.app.json_encoder = encoder.JSONEncoder
    app.add_api('swagger.yaml', arguments={'title': 'In-Situ Pipeline REST API'})
    app.run(port=8080)


if __name__ == '__main__':
    main()
