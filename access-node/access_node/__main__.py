#!/usr/bin/env python3

import connexion
import sys

from access_node import encoder

from access_node.models.simulation_nodes import simulation_nodes


def load_simulation_nodes(path):
    try:
        with open(path) as simulation_nodes_file:
            for simulation_node in simulation_nodes_file:
                nodes.nest_simulation_nodes.append(simulation_node.rstrip('\n'))
    except:
        pass

def main():
    if len(sys.argv) == 1:
        load_simulation_nodes("simulation_nodes.txt")
    else:
        simulation_nodes.nest_simulation_nodes = sys.argv[1:]

    app = connexion.App("access_node", specification_dir='./openapi/')
    # run access_node
    app.app.json_encoder = encoder.JSONEncoder
    app.add_api('v1.yaml',
                arguments={'title': 'Insite'},
                pythonic_params=True)
    app.add_api('v1.yaml',
                arguments={'title': 'Insite'},
                pythonic_params=True,
                base_path='/')
    app.run(port=8080)


if __name__ == '__main__':
    main()
