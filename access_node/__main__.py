#!/usr/bin/env python3

import connexion

from access_node import encoder
from flask_cors import CORS

from access_node.models.nodes import nodes
import json

import time
import requests
import psycopg2

def ConnectToDatabase(postgres_username, postgres_password):
	database_host = 'database'
    with open('database_host.txt') as database_host_file:
        database_host = database_host_file.readline()
    return psycopg2.connect(database="postgres", user="postgres",
                       password="postgres", host=database_host, port="5432")


def main():
	# Wait for simulation nodes to post to database
	time.sleep(5)

	# get simulation nodes
	con = ConnectToDatabase('postgres', 'postgres')
	cur = con.cursor()
	# NEST
	cur.execute("SELECT address FROM nest_simulation_node")
	nodes.nest_simulation_nodes = [i[0] for i in cur.fetchall()]
	# Arbor
	cur.execute("SELECT address FROM nest_simulation_node")
	nodes.arbor_simulation_nodes = [i[0] for i in cur.fetchall()]
	con.close()


	# run acces_node
	app = connexion.App(__name__, specification_dir='./swagger/')
	app.app.json_encoder = encoder.JSONEncoder
	app.add_api('swagger.yaml', arguments={
				'title': 'In-Situ Pipeline REST API'})
	CORS(app.app)
	app.run(port=8080)


if __name__ == '__main__':
    main()
