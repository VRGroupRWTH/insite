# In-Situ Pipeline REST API
This is the REST API for the in-situ pipeline.

## Version: 0.1.0

### /arbor/spikes

#### GET
##### Summary:

Retrieves the spikes for the given simulation times (optional),  cell and segment (optional).

##### Parameters

| Name | Located in | Description | Required | Schema |
| ---- | ---------- | ----------- | -------- | ---- |
| from | query | The start time (including) to be queried. | No | double |
| to | query | The end time (excluding) to be queried. | No | double |
| cell_ids | query | A list of cell ids queried for spike data. | No | [ integer (uint64) ] |
| segment_ids | query | A list of segment ids queried for spike data. | No | [ integer (uint64) ] |
| offset | query | The offset into the result. | No | integer (uint64) |
| limit | query | The maximum of entries to be result. | No | integer (uint64) |

##### Responses

| Code | Description | Schema |
| ---- | ----------- | ------ |
| 200 | Operation successful. | [Spikes](#spikes) |
| 400 | Operation failed. | string |

### /arbor/cell_ids

#### GET
##### Summary:

Retrieves a list of all cell ids.

##### Parameters

| Name | Located in | Description | Required | Schema |
| ---- | ---------- | ----------- | -------- | ---- |

##### Responses

| Code | Description | Schema |
| ---- | ----------- | ------ |
| 200 | Operation successful. | [ integer (uint64) ] |
| 400 | Operation failed. | string |

### /arbor/probes

#### GET
##### Summary:

Retrieves a list of all probes for a given attribute (optional).

##### Parameters

| Name | Located in | Description | Required | Schema |
| ---- | ---------- | ----------- | -------- | ---- |
| attribute | query | The attribute measured for which existing probes will be returned. | No | string |

##### Responses

| Code | Description | Schema |
| ---- | ----------- | ------ |
| 200 | Operation successful. | [ [Probe](#probe) ] |
| 400 | Operation failed. | string |

### /arbor/attributes

#### GET
##### Summary:

Retrieves a list of all attributes.

##### Parameters

| Name | Located in | Description | Required | Schema |
| ---- | ---------- | ----------- | -------- | ---- |

##### Responses

| Code | Description | Schema |
| ---- | ----------- | ------ |
| 200 | Operation successful. | [ string ] |
| 400 | Operation failed. | string |

### /arbor/measurements

#### GET
##### Summary:

Retrieves the measurements for given probes (optional).

##### Parameters

| Name | Located in | Description | Required | Schema |
| ---- | ---------- | ----------- | -------- | ---- |
| attribute | query | The attribute to query | Yes | string |
| probe_ids | query | A list of probes ids queried for data. | No | [ integer (uint64) ] |
| from | query | The start time (including) to be queried. | No | double |
| to | query | The end time (excluding) to be queried. | No | double |
| offset | query | The offset into the result. | No | integer (uint64) |
| limit | query | The maximum of entries to be returned. | No | integer (uint64) |

##### Responses

| Code | Description | Schema |
| ---- | ----------- | ------ |
| 200 | Operation successful. | [ArborMeasurement](#arbormeasurement) |
| 400 | Operation failed. | string |

### /arbor/simulation_time_info

#### GET
##### Summary:

Retrieves simulation time information(begin, current, end).

##### Parameters

| Name | Located in | Description | Required | Schema |
| ---- | ---------- | ----------- | -------- | ---- |

##### Responses

| Code | Description | Schema |
| ---- | ----------- | ------ |
| 200 | Operation successful. | [SimulationTimeInfo](#simulationtimeinfo) |
| 400 | Operation failed. | string |

### /arbor/cell_properties

#### GET
##### Summary:

Retrieves the properties of the specified cells.

##### Parameters

| Name | Located in | Description | Required | Schema |
| ---- | ---------- | ----------- | -------- | ---- |
| cell_ids | query | A list of cell IDs queried for properties. | No | [ integer (uint64) ] |

##### Responses

| Code | Description | Schema |
| ---- | ----------- | ------ |
| 200 | Operation successful. | [ [ArborCellProperties](#arborcellproperties) ] |
| 400 | Operation failed. | string |

### /nest/simulation_time_info

#### GET
##### Summary:

Retrieves simulation time information (step_size, begin, current, end).

##### Parameters

| Name | Located in | Description | Required | Schema |
| ---- | ---------- | ----------- | -------- | ---- |

##### Responses

| Code | Description | Schema |
| ---- | ----------- | ------ |
| 200 | Operation successful. | [SimulationTimeInfo](#simulationtimeinfo) |
| 400 | Operation failed. | string |

### /nest/gids

#### GET
##### Summary:

Retrieves a list of all gids.

##### Parameters

| Name | Located in | Description | Required | Schema |
| ---- | ---------- | ----------- | -------- | ---- |

##### Responses

| Code | Description | Schema |
| ---- | ----------- | ------ |
| 200 | Operation successful. | [ integer (uint64) ] |
| 400 | Operation failed. | string |

### /nest/neuron_properties

#### GET
##### Summary:

Retrieves the properties of the specified neurons.

##### Parameters

| Name | Located in | Description | Required | Schema |
| ---- | ---------- | ----------- | -------- | ---- |
| gids | query | A list of gids queried for properties. | No | [ integer (uint64) ] |

##### Responses

| Code | Description | Schema |
| ---- | ----------- | ------ |
| 200 | Operation successful. | [ [NestNeuronProperties](#nestneuronproperties) ] |
| 400 | Operation failed. | string |

### /nest/populations

#### GET
##### Summary:

Retrieves a list of all population IDs.

##### Parameters

| Name | Located in | Description | Required | Schema |
| ---- | ---------- | ----------- | -------- | ---- |

##### Responses

| Code | Description | Schema |
| ---- | ----------- | ------ |
| 200 | Operation successful. | [ integer (uint64) ] |
| 400 | Operation failed. | string |

### /nest/spikes

#### GET
##### Summary:

Retrieves the spikes for the given simulation steps (optional) and gids (optional).

##### Parameters

| Name | Located in | Description | Required | Schema |
| ---- | ---------- | ----------- | -------- | ---- |
| from | query | The start time (including) to be queried. | No | double |
| to | query | The end time (excluding) to be queried. | No | double |
| gids | query | A list of gids queried for spike data. | No | [ integer (uint64) ] |
| offset | query | The offset into the result. | No | integer (uint64) |
| limit | query | The maximum of entries to be returned. | No | integer (uint64) |

##### Responses

| Code | Description | Schema |
| ---- | ----------- | ------ |
| 200 | Operation successful. | [Spikes](#spikes) |
| 400 | Operation failed. | string |

### /nest/population/${population_id}/spikes

#### GET
##### Summary:

Retrieves the spikes for the given simulation steps (optional) and population.

##### Parameters

| Name | Located in | Description | Required | Schema |
| ---- | ---------- | ----------- | -------- | ---- |
| population_id | path | The identifier of the population. | Yes | integer (uint64) |
| from | query | The start time (including) to be queried. | No | double |
| to | query | The end time (excluding) to be queried. | No | double |
| offset | query | The offset into the result. | No | integer (uint64) |
| limit | query | The maximum of entries to be returned. | No | integer (uint64) |

##### Responses

| Code | Description | Schema |
| ---- | ----------- | ------ |
| 200 | Operation successful. | [Spikes](#spikes) |
| 400 | Operation failed. | string |

### /nest/multimeter_info

#### GET
##### Summary:

Retreives the available multimeters and their properties.

##### Parameters

| Name | Located in | Description | Required | Schema |
| ---- | ---------- | ----------- | -------- | ---- |

##### Responses

| Code | Description | Schema |
| ---- | ----------- | ------ |
| 200 | Operation successful. | [MultimeterInfo](#multimeterinfo) |
| 400 | Operation failed. | string |

### /nest/multimeter_measurement

#### GET
##### Summary:

Retrieves the measurements for a multimeter, attribute and gids (optional).

##### Parameters

| Name | Located in | Description | Required | Schema |
| ---- | ---------- | ----------- | -------- | ---- |
| multimeter_id | query | The multimeter to query | Yes | integer (uint64) |
| attribute | query | The attribute to query (e.g., 'V_m' for the membrane potential) | Yes | string |
| from | query | The start time (including) to be queried. | No | double |
| to | query | The end time (excluding) to be queried. | No | double |
| gids | query | A list of gids queried for spike data. | No | [ integer (uint64) ] |
| offset | query | The offset into the result. | No | integer (uint64) |
| limit | query | The maximum of entries to be result. | No | integer (uint64) |

##### Responses

| Code | Description | Schema |
| ---- | ----------- | ------ |
| 200 | Operation successful. | [MultimeterMeasurement](#multimetermeasurement) |
| 400 | Operation failed. | string |

### /nest/population/${population_id}/gids

#### GET
##### Summary:

Retrieves the list of all neuron ids within the population.

##### Parameters

| Name | Located in | Description | Required | Schema |
| ---- | ---------- | ----------- | -------- | ---- |
| population_id | path | The identifier of the population | Yes | integer (uint64) |

##### Responses

| Code | Description | Schema |
| ---- | ----------- | ------ |
| 200 | Operation successful. | [ integer (uint64) ] |
| 400 | Operation failed. | string |

### Models


#### Spikes

| Name | Type | Description | Required |
| ---- | ---- | ----------- | -------- |
| simulation_times | [ double ] | This array is always sorted. | No |
| gids | [ integer (uint64) ] |  | No |

#### MultimeterInfo

| Name | Type | Description | Required |
| ---- | ---- | ----------- | -------- |
| MultimeterInfo | array |  |  |

#### MultimeterMeasurement

| Name | Type | Description | Required |
| ---- | ---- | ----------- | -------- |
| simulation_times | [ double ] | This array is always sorted. | No |
| gids | [ integer (uint64) ] |  | No |
| values | [ number ] | This array contains the measured values for each gid and time to get the value for gid n at time t you have to use the index n * length(simulation_times) + t | No |

#### ArborMeasurement

| Name | Type | Description | Required |
| ---- | ---- | ----------- | -------- |
| simulation_times | [ double ] | This array is always sorted. | No |
| gids | [ integer (uint64) ] |  | No |
| values | [ number ] | This array contains the measured values for each gid and time to get the value for gid n at time t you have to use the index n * length(simulation_times) + t | No |

#### Probe

| Name | Type | Description | Required |
| ---- | ---- | ----------- | -------- |
| id | integer (uint64) |  | No |
| cell_id | integer (uint64) |  | No |
| segment_id | integer (uint64) |  | No |
| position | double |  | No |

#### ArborCellProperties

| Name | Type | Description | Required |
| ---- | ---- | ----------- | -------- |
| neuron_id | integer (uint64) |  | No |
| properties | object |  | No |

#### NestNeuronProperties

| Name | Type | Description | Required |
| ---- | ---- | ----------- | -------- |
| gid | integer (uint64) |  | No |
| properties | object |  | No |

#### SimulationTimeInfo

| Name | Type | Description | Required |
| ---- | ---- | ----------- | -------- |
| step_size | double |  | No |
| begin | double |  | No |
| end | double |  | No |
| current | double |  | No |

#### MultimeterInfo_inner

| Name | Type | Description | Required |
| ---- | ---- | ----------- | -------- |
| id | integer (uint64) |  | No |
| attributes | [ string ] |  | No |
| gids | [ integer (uint64) ] |  | No |