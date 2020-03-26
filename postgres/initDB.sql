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
osition           FLOAT[],
     FOREIGN KEY (simulation_node_id) REFERENCES nest_simulation_node (id)
);

CREATE TABLE nest_neuron_multimeter (
     neuron_id     INT NOT NULL,
     multimeter_id INT NOT NULL,
     PRIMARY KEY (neuron_id,multimeter_id),
     FOREIGN KEY (neuron_id) REFERENCES nest_neuron (id),
     FOREIGN KEY (multimeter_id) REFERENCES nest_multimeter (id)
);