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
	cur.execute("DROP TABLE IF EXISTS SIMULATION_NODES CASCADE")
	cur.execute("DROP TABLE IF EXISTS MULTIMETERS CASCADE")
	cur.execute("DROP TABLE IF EXISTS GIDS CASCADE")
	cur.execute("DROP TABLE IF EXISTS MULT_PER_GID CASCADE")


	cur.execute('''CREATE TABLE SIMULATION_NODES (
      NODE_ID           SERIAL PRIMARY KEY NOT NULL UNIQUE,
      ADDRESS           VARCHAR(50),
      CURRENT_SIM_TIME  FLOAT);''')

	cur.execute('''CREATE TABLE MULTIMETERS (
      MULTIMETER_ID   INT PRIMARY KEY NOT NULL UNIQUE,
      ATTRIBUTE       VARCHAR(50) );''')

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
	con = ConnectToDatabase(postgres_username, postgres_password, port)
	print("Database connection opened successfully!")
	cur = con.cursor()
	cur.execute("DROP TABLE IF EXISTS PROBES CASCADE")
	cur.execute("DROP TABLE IF EXISTS CELLS CASCADE")
	cur.execute("DROP TABLE IF EXISTS ARBOR_SIMULATION_NODES CASCADE")
	cur.execute("DROP TABLE IF EXISTS ATTRIBUTES CASCADE")

	cur.execute('''CREATE TABLE ARBOR_SIMULATION_NODES (
      NODE_ID           INT PRIMARY KEY NOT NULL UNIQUE,
      ADDRESS           VARCHAR(50),
      CURRENT_SIM_TIME  FLOAT);''')

	cur.execute('''CREATE TABLE CELLS (
      CELL_ID   			INT PRIMARY KEY NOT NULL UNIQUE
			);''')

	cur.execute('''CREATE TABLE CELL_PROPERTIES (
      CELL_ID   			INT NOT NULL,
			PROPERTY				VARCHAR(50) NOT NULL,
			PRIMARY KEY (CELL_ID, PROPERTY),
			FOREIGN KEY (CELL_ID) REFERENCES CELLS (CELL_ID),
			);''')

	cur.execute('''CREATE TABLE ATTRIBUTES (
      ATTRIBUTE_ID		INT PRIMARY KEY NOT NULL UNIQUE,
      NAME   					VARCHAR(50) NOT NULL,
    	);''')

	cur.execute('''CREATE TABLE PROBES (
      PROBE_ID      INT PRIMARY KEY NOT NULL UNIQUE,
      CELL_ID       INT NOT NULL,  
      SEGMENT_ID 		INT NOT NULL,
			POSITION   		FLOAT,
			ATTRIBUTE_ID	INT NOT NULL,
			NODE_ID       INT,
      FOREIGN KEY (NODE_ID) REFERENCES ARBOR_SIMULATION_NODES (NODE_ID),
			FOREIGN KEY (CELL_ID) REFERENCES CELLS (CELL_ID),
			FOREIGN KEY (ATTRIBUTE_ID) REFERENCES ATTRIBUTES (ATTRIBUTE_ID));''')

	

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
	cur.execute("SELECT ADDRESS FROM SIMULATION_NODES")
	nodes.nest_simulation_nodes = [i[0] for i in cur.fetchall()]
	# Arbor
	#cur.execute("SELECT ADDRESS FROM SIMULATION_NODES")
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
