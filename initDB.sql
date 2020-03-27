DROP TABLE IF EXISTS nest_simulation_node CASCADE;
DROP TABLE IF EXISTS nest_multimeter CASCADE;
DROP TABLE IF EXISTS nest_neuron CASCADE;
DROP TABLE IF EXISTS nest_neuron_multimeter CASCADE;


CREATE TABLE nest_simulation_node (
  id                      SERIAL PRIMARY KEY NOT NULL UNIQUE,
  address                 VARCHAR(50),
  current_simulation_time FLOAT
);

CREATE TABLE nest_multimeter (
  id         INT PRIMARY KEY NOT NULL UNIQUE,
  attributes VARCHAR(50) ARRAY
);

CREATE TABLE nest_neuron (
  id                 INT PRIMARY KEY NOT NULL UNIQUE,
  simulation_node_id INT,  
  population_id      INT,
  position           FLOAT[],
  FOREIGN KEY (simulation_node_id) REFERENCES nest_simulation_node (id)
);

CREATE TABLE nest_neuron_multimeter (
  neuron_id     INT NOT NULL,
  multimeter_id INT NOT NULL,
  PRIMARY KEY (neuron_id,multimeter_id),
  FOREIGN KEY (neuron_id) REFERENCES nest_neuron (id),
  FOREIGN KEY (multimeter_id) REFERENCES nest_multimeter (id)
);




DROP TABLE IF EXISTS arbor_probe CASCADE;
DROP TABLE IF EXISTS arbor_cell CASCADE;
DROP TABLE IF EXISTS arbor_simulation_node CASCADE;
DROP TABLE IF EXISTS arbor_attribute CASCADE;

CREATE TABLE arbor_simulation_node (
  id                       INT PRIMARY KEY NOT NULL UNIQUE,
  address                  VARCHAR(50),
  current_simulation_time  FLOAT
);

CREATE TABLE arbor_cell (
  id INT PRIMARY KEY NOT NULL UNIQUE
);

CREATE TABLE cell_property (
  cell_id  INT NOT NULL,
	property VARCHAR(50) NOT NULL,
	PRIMARY KEY (cell_id, property),
	FOREIGN KEY (cell_id) REFERENCES arbor_cell (id)
);

CREATE TABLE arbor_attribute (
  id   INT PRIMARY KEY NOT NULL UNIQUE,
  name VARCHAR(50) NOT NULL
);

CREATE TABLE arbor_probe (
  id               INT PRIMARY KEY NOT NULL UNIQUE,
  cell_id          INT NOT NULL,  
  segment_id 		   INT NOT NULL,
	position   		   FLOAT,
	attribute_id	   INT NOT NULL,
	simulation_node_id INT,
  FOREIGN KEY (simulation_node_id) REFERENCES arbor_simulation_node (id),
	FOREIGN KEY (cell_id) REFERENCES arbor_cell (id),
	FOREIGN KEY (attribute_id) REFERENCES arbor_attribute (id)
);