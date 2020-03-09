#!/usr/bin/env python3

import connexion

from access_node import encoder
from flask_cors import CORS

from access_node.models.nodes import nodes
import json

import time
import requests
import psycopg2

def ConnectToDatabase(postgres_username, postgres_password, port):
	return psycopg2.connect(database="postgres", user=postgres_username,
                       password=postgres_password, host="database", port=str(port))

def SetupNestTables(postgres_username, postgres_password, port):
	con = ConnectToDatabase(postgres_username, postgres_password, port)
	print("Database connection opened successfully!")

	cur = con.cursor()
	cur.execute("DROP TABLE IF EXISTS nest_simulation_node CASCADE")
	cur.execute("DROP TABLE IF EXISTS nest_multimeter CASCADE")
	cur.execute("DROP TABLE IF EXISTS nest_neuron CASCADE")
	cur.execute("DROP TABLE IF EXISTS nest_neuron_multimeter CASCADE")


	cur.execute('''
	  CREATE TABLE nest_simulation_node (
        id                      SERIAL PRIMARY KEY NOT NULL UNIQUE,
        address                 VARCHAR(50),
        current_simulation_time FLOAT
	  );''')

	cur.execute('''
	  CREATE TABLE nest_multimeter (
        id        INT PRIMARY KEY NOT NULL UNIQUE,
        attribute VARCHAR(50)
	  );''')

	cur.execute('''
	  CREATE TABLE nest_neuron (
        id                 INT PRIMARY KEY NOT NULL UNIQUE,
        simulation_node_id INT,  
        population_id      INT,
        FOREIGN KEY (simulation_node_id) REFERENCES nest_simulation_node (id)
	  );''')

	cur.execute('''
	  CREATE TABLE nest_neuron_multimeter (
        neuron_id     INT NOT NULL,
        multimeter_id INT NOT NULL,
        PRIMARY KEY (neuron_id,multimeter_id),
        FOREIGN KEY (neuron_id) REFERENCES nest_neuron (id),
        FOREIGN KEY (multimeter_id) REFERENCES nest_multimeter (id)
	  );''')

	con.commit()
	con.close()
	print("Nest tables created successfully!\n")

def SetupArborTables(postgres_username, postgres_password, port):
	con = ConnectToDatabase(postgres_username, postgres_password, port)
	print("Database connection opened successfully!")
	cur = con.cursor()
	cur.execute("DROP TABLE IF EXISTS arbor_probe CASCADE")
	cur.execute("DROP TABLE IF EXISTS arbor_cell CASCADE")
	cur.execute("DROP TABLE IF EXISTS arbor_simulation_node CASCADE")
	cur.execute("DROP TABLE IF EXISTS arbor_attribute CASCADE")

	cur.execute('''
	  CREATE TABLE arbor_simulation_node (
        id                       INT PRIMARY KEY NOT NULL UNIQUE,
        address                  VARCHAR(50),
        current_simulation_time  FLOAT
	  );''')

	cur.execute('''
	  CREATE TABLE arbor_cell (
        id INT PRIMARY KEY NOT NULL UNIQUE
	  );
	  ''')

	cur.execute('''
	  CREATE TABLE cell_property (
        cell_id  INT NOT NULL,
		property VARCHAR(50) NOT NULL,
		PRIMARY KEY (cell_id, property),
		FOREIGN KEY (cell_id) REFERENCES arbor_cell (id),
	  );''')

	cur.execute('''
	  CREATE TABLE arbor_attribute (
        id   INT PRIMARY KEY NOT NULL UNIQUE,
        name VARCHAR(50) NOT NULL,
	  );''')

	cur.execute('''
	  CREATE TABLE arbor_probe (
        id                 INT PRIMARY KEY NOT NULL UNIQUE,
        cell_id            INT NOT NULL,  
        segment_id 		   INT NOT NULL,
		position   		   FLOAT,
		attribute_id	   INT NOT NULL,
		simulation_node_id INT,
        FOREIGN KEY (simulation_node_id) REFERENCES arbor_simulation_node (id),
		FOREIGN KEY (cell_id) REFERENCES arbor_cell (id),
		FOREIGN KEY (attribute_id) REFERENCES arbor_attribute (id)
	  );''')

	

	con.commit()
	con.close()
	print("Arbor tables created successfully!\n")


def main():
	# Connect to the Database and initalize basic Table structure
	SetupNestTables('postgres', 'docker', 5432)
	#SetupArborTables('postgres', 'docker', 5432)

	# Wait for simulation nodes to post to database
	time.sleep(5)


	# get simulation nodes
	con = ConnectToDatabase('postgres', 'docker', 5432)
	cur = con.cursor()
	# NEST
	cur.execute("SELECT address FROM nest_simulation_node")
	nodes.nest_simulation_nodes = [i[0] for i in cur.fetchall()]
	# Arbor
	#cur.execute("SELECT address FROM nest_simulation_node")
	#nodes.nest_simulation_nodes = [i[0] for i in cur.fetchall()]
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
