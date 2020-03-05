#!/usr/bin/env python3

import connexion

from access_node import encoder
from flask_cors import CORS

from access_node.models.nodes import nodes
import json

import requests
import psycopg2


def SetupNestTables(postgres_username, postgres_password, port):
	con = psycopg2.connect(database="postgres", user=postgres_username,
                       password=postgres_password, host="database", port=str(port))
	print("Database connection opened successfully!")

	cur = con.cursor()
	cur.execute("DROP TABLE IF EXISTS SIMULATION_NODES CASCADE")
	cur.execute("DROP TABLE IF EXISTS MULTIMETERS CASCADE")
	cur.execute("DROP TABLE IF EXISTS GIDS CASCADE")
	cur.execute("DROP TABLE IF EXISTS MULT_PER_GID CASCADE")


	cur.execute('''CREATE TABLE SIMULATION_NODES (
      NODE_ID           INT       PRIMARY KEY NOT NULL UNIQUE,
      ADDRESS           VARCHAR(25),
      CURRENT_SIM_TIME  FLOAT);''')

	cur.execute('''CREATE TABLE MULTIMETERS (
      MULTIMETER_ID   INT PRIMARY KEY NOT NULL UNIQUE,
      ATTRIBUTE       CHAR(50) );''')

	cur.execute('''CREATE TABLE GIDS (
      GID             INT PRIMARY KEY NOT NULL UNIQUE,
      NODE_ID         INT,  
      POPULATION_ID   INT,
      FOREIGN KEY (NODE_ID) REFERENCES SIMULATION_NODES (NODE_ID));''')

	cur.execute('''CREATE TABLE MULT_PER_GID(
      GID             INT NOT NULL,
      MULTIMETER_ID   INT NOT NULL,
      PRIMARY KEY (GID,MULTIMETER_ID),
      FOREIGN KEY (GID) REFERENCES GIDS (GID),
      FOREIGN KEY (MULTIMETER_ID) REFERENCES MULTIMETERS (MULTIMETER_ID));''')

	con.commit()
	con.close()
	print("Nest tables created successfully!\n")

def SetupArborTables(postgres_username, postgres_password, port):
	con = psycopg2.connect(database="postgres", user=postgres_username,
                      password=postgres_password, host="database", port=str(port))
	print("Database connection opened successfully!")
	cur = con.cursor()
	cur.execute("DROP TABLE IF EXISTS PROBES CASCADE")
	cur.execute("DROP TABLE IF EXISTS CELLS CASCADE")
	cur.execute("DROP TABLE IF EXISTS SIMULATION_NODES CASCADE")
	cur.execute("DROP TABLE IF EXISTS ATTRIBUTES CASCADE")
	cur.execute('''CREATE TABLE SIMULATION_NODES (
      NODE_ID           INT       PRIMARY KEY NOT NULL UNIQUE,
      ADDRESS           VARCHAR(25),
      CURRENT_SIM_TIME  FLOAT);''')
	cur.execute('''CREATE TABLE CELLS (
      CELL_ID   			INT PRIMARY KEY NOT NULL UNIQUE
			);''')
	cur.execute('''CREATE TABLE CELL_PROPERTIES (
      CELL_ID   			INT NOT NULL,
			PROPERTY				VARCHAR(50),
			PRIMARY KEY (CELL_ID, PROPERTY),
			FOREIGN KEY (CELL_ID) REFERENCES CELLS (CELL_ID),
			);''')
	cur.execute('''CREATE TABLE PROBES (
      PROBE_ID      INT PRIMARY KEY NOT NULL UNIQUE,
      CELL_ID       INT,  
      SEGMENT_ID 		INT,
			POSITION   		FLOAT,
			NODE_ID       INT,  
      FOREIGN KEY (NODE_ID) REFERENCES SIMULATION_NODES (NODE_ID),
			FOREIGN KEY (CELL_ID) REFERENCES CELLS (CELL_ID));''')
	cur.execute('''CREATE TABLE ATTRIBUTES (
      ATTRIBUTE_ID		INT PRIMARY KEY NOT NULL UNIQUE,
      NAME   					INT NOT NULL,
    	);''')
	con.commit()
	con.close()
	print("Arbor tables created successfully!\n")


def main():
	# Connect to the Database and initalize basic Table structure
	SetupNestTables('postgres', 'docker', 5432)

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
