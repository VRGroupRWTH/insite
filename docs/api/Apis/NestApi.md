# NestApi

All URIs are relative to *http://localhost/v1*

Method | HTTP request | Description
------------- | ------------- | -------------
[**nestGetKernelStatus**](NestApi.md#nestGetKernelStatus) | **GET** /nest/kernelStatus | Retreives the current status of the NEST kernel.
[**nestGetMultimeterById**](NestApi.md#nestGetMultimeterById) | **GET** /nest/multimeters/{multimeterId} | Retreives the available multimeters and their properties.
[**nestGetMultimeterMeasurements**](NestApi.md#nestGetMultimeterMeasurements) | **GET** /nest/multimeters/{multimeterId}/attributes/{attributeName} | Retrieves the measurements for a multimeter, attribute and node IDs (optional).
[**nestGetMultimeters**](NestApi.md#nestGetMultimeters) | **GET** /nest/multimeters | Retreives the available multimeters and their properties.
[**nestGetNodeById**](NestApi.md#nestGetNodeById) | **GET** /nest/nodes/{nodeId} | Retrieves the properties of the specified node.
[**nestGetNodeCollections**](NestApi.md#nestGetNodeCollections) | **GET** /nest/nodeCollections | Retrieves a list of all node collection IDs.
[**nestGetNodeIds**](NestApi.md#nestGetNodeIds) | **GET** /nest/nodes/ids | Retrieves a list of all node IDs.
[**nestGetNodeIdsByNodeCollection**](NestApi.md#nestGetNodeIdsByNodeCollection) | **GET** /nest/nodeCollections/${nodeCollectionId}/nodes/ids | Retrieves the list of all node ids within the node collection.
[**nestGetNodes**](NestApi.md#nestGetNodes) | **GET** /nest/nodes | Retrieves all nodes of the simulation.
[**nestGetNodesByNodeCollection**](NestApi.md#nestGetNodesByNodeCollection) | **GET** /nest/nodeCollections/${nodeCollectionId}/nodes | Retrieves the list of all node within the node collection.
[**nestGetSimulationTimeInfo**](NestApi.md#nestGetSimulationTimeInfo) | **GET** /nest/simulationTimeInfo | Retrieves simulation time information (stepSize, begin, current, end).
[**nestGetSpikedetectors**](NestApi.md#nestGetSpikedetectors) | **GET** /nest/spikedetectors/ | Queries all spike detectors accessable to the pipeline.
[**nestGetSpikes**](NestApi.md#nestGetSpikes) | **GET** /nest/spikes | Retrieves the spikes for the given time range (optional) and node IDs (optional). If no time range or node list is specified, it will return the spikes for whole time or all nodes respectively. This request merges the spikes recorded by all spike detectors and removes duplicates.
[**nestGetSpikesByNodeCollection**](NestApi.md#nestGetSpikesByNodeCollection) | **GET** /nest/nodeCollections/{nodeCollectionId}/spikes | Retrieves the spikes for the given simulation steps (optional) and node collection. This request merges the spikes recorded by all spike detectors and removes duplicates.
[**nestGetSpikesBySpikedetector**](NestApi.md#nestGetSpikesBySpikedetector) | **GET** /nest/spikedetectors/{spikedetectorId}/spikes | Retrieves the spikes for the given time range (optional) and node IDs (optional) from one spike detector. If no time range or node list is specified, it will return the spikes for whole time or all nodes respectively.


<a name="nestGetKernelStatus"></a>
# **nestGetKernelStatus**
> List nestGetKernelStatus()

Retreives the current status of the NEST kernel.

### Parameters
This endpoint does not need any parameter.

### Return type

[**List**](..//Models/MultimeterInfo.md)

### Authorization

No authorization required

### HTTP request headers

- **Content-Type**: Not defined
- **Accept**: application/json

<a name="nestGetMultimeterById"></a>
# **nestGetMultimeterById**
> MultimeterInfo nestGetMultimeterById(multimeterId)

Retreives the available multimeters and their properties.

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **multimeterId** | **Integer**| The identifier of the multimeter. | [default to null]

### Return type

[**MultimeterInfo**](..//Models/MultimeterInfo.md)

### Authorization

No authorization required

### HTTP request headers

- **Content-Type**: Not defined
- **Accept**: application/json

<a name="nestGetMultimeterMeasurements"></a>
# **nestGetMultimeterMeasurements**
> MultimeterMeasurement nestGetMultimeterMeasurements(multimeterId, attributeName, fromTime, toTime, nodeIds, skip, top)

Retrieves the measurements for a multimeter, attribute and node IDs (optional).

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **multimeterId** | **Integer**| The multimeter to query | [default to null]
 **attributeName** | **String**| The attribute to query (e.g., &#39;V_m&#39; for the membrane potential) | [default to null]
 **fromTime** | **Double**| The start time (including) to be queried. | [optional] [default to null]
 **toTime** | **Double**| The end time (excluding) to be queried. | [optional] [default to null]
 **nodeIds** | [**List**](..//Models/Integer.md)| A list of node IDs queried for attribute data. | [optional] [default to null]
 **skip** | **Integer**| The offset into the result. | [optional] [default to null]
 **top** | **Integer**| The maximum number of entries to be returned. | [optional] [default to null]

### Return type

[**MultimeterMeasurement**](..//Models/MultimeterMeasurement.md)

### Authorization

No authorization required

### HTTP request headers

- **Content-Type**: Not defined
- **Accept**: application/json

<a name="nestGetMultimeters"></a>
# **nestGetMultimeters**
> List nestGetMultimeters()

Retreives the available multimeters and their properties.

### Parameters
This endpoint does not need any parameter.

### Return type

[**List**](..//Models/MultimeterInfo.md)

### Authorization

No authorization required

### HTTP request headers

- **Content-Type**: Not defined
- **Accept**: application/json

<a name="nestGetNodeById"></a>
# **nestGetNodeById**
> NestNodeProperties nestGetNodeById(nodeId)

Retrieves the properties of the specified node.

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **nodeId** | **Integer**| The ID of the queried node. | [default to null]

### Return type

[**NestNodeProperties**](..//Models/NestNodeProperties.md)

### Authorization

No authorization required

### HTTP request headers

- **Content-Type**: Not defined
- **Accept**: application/json

<a name="nestGetNodeCollections"></a>
# **nestGetNodeCollections**
> List nestGetNodeCollections()

Retrieves a list of all node collection IDs.

### Parameters
This endpoint does not need any parameter.

### Return type

[**List**](..//Models/NestNodeCollectionProperties.md)

### Authorization

No authorization required

### HTTP request headers

- **Content-Type**: Not defined
- **Accept**: application/json

<a name="nestGetNodeIds"></a>
# **nestGetNodeIds**
> List nestGetNodeIds()

Retrieves a list of all node IDs.

### Parameters
This endpoint does not need any parameter.

### Return type

[**List**](..//Models/integer.md)

### Authorization

No authorization required

### HTTP request headers

- **Content-Type**: Not defined
- **Accept**: application/json

<a name="nestGetNodeIdsByNodeCollection"></a>
# **nestGetNodeIdsByNodeCollection**
> List nestGetNodeIdsByNodeCollection(nodeCollectionId)

Retrieves the list of all node ids within the node collection.

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **nodeCollectionId** | **Integer**| The identifier of the node collection | [default to null]

### Return type

[**List**](..//Models/integer.md)

### Authorization

No authorization required

### HTTP request headers

- **Content-Type**: Not defined
- **Accept**: application/json

<a name="nestGetNodes"></a>
# **nestGetNodes**
> List nestGetNodes()

Retrieves all nodes of the simulation.

### Parameters
This endpoint does not need any parameter.

### Return type

[**List**](..//Models/NestNodeProperties.md)

### Authorization

No authorization required

### HTTP request headers

- **Content-Type**: Not defined
- **Accept**: application/json

<a name="nestGetNodesByNodeCollection"></a>
# **nestGetNodesByNodeCollection**
> List nestGetNodesByNodeCollection(nodeCollectionId)

Retrieves the list of all node within the node collection.

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **nodeCollectionId** | **Integer**| The identifier of the node collection | [default to null]

### Return type

[**List**](..//Models/NestNodeProperties.md)

### Authorization

No authorization required

### HTTP request headers

- **Content-Type**: Not defined
- **Accept**: application/json

<a name="nestGetSimulationTimeInfo"></a>
# **nestGetSimulationTimeInfo**
> SimulationTimeInfo nestGetSimulationTimeInfo()

Retrieves simulation time information (stepSize, begin, current, end).

### Parameters
This endpoint does not need any parameter.

### Return type

[**SimulationTimeInfo**](..//Models/SimulationTimeInfo.md)

### Authorization

No authorization required

### HTTP request headers

- **Content-Type**: Not defined
- **Accept**: application/json

<a name="nestGetSpikedetectors"></a>
# **nestGetSpikedetectors**
> List nestGetSpikedetectors()

Queries all spike detectors accessable to the pipeline.

### Parameters
This endpoint does not need any parameter.

### Return type

[**List**](..//Models/SpikedetectorInfo.md)

### Authorization

No authorization required

### HTTP request headers

- **Content-Type**: Not defined
- **Accept**: application/json

<a name="nestGetSpikes"></a>
# **nestGetSpikes**
> Spikes nestGetSpikes(fromTime, toTime, nodeIds, skip, top)

Retrieves the spikes for the given time range (optional) and node IDs (optional). If no time range or node list is specified, it will return the spikes for whole time or all nodes respectively. This request merges the spikes recorded by all spike detectors and removes duplicates.

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **fromTime** | **Double**| The start time in milliseconds (including) to be queried. | [optional] [default to null]
 **toTime** | **Double**| The end time in milliseconds (excluding) to be queried. | [optional] [default to null]
 **nodeIds** | [**List**](..//Models/Integer.md)| A list of node IDs queried for spike data. | [optional] [default to null]
 **skip** | **Integer**| The offset into the result. | [optional] [default to null]
 **top** | **Integer**| The maximum number of entries to be returned. | [optional] [default to null]

### Return type

[**Spikes**](..//Models/Spikes.md)

### Authorization

No authorization required

### HTTP request headers

- **Content-Type**: Not defined
- **Accept**: application/json

<a name="nestGetSpikesByNodeCollection"></a>
# **nestGetSpikesByNodeCollection**
> Spikes nestGetSpikesByNodeCollection(nodeCollectionId, fromTime, toTime, skip, top)

Retrieves the spikes for the given simulation steps (optional) and node collection. This request merges the spikes recorded by all spike detectors and removes duplicates.

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **nodeCollectionId** | **Integer**| The identifier of the node collection. | [default to null]
 **fromTime** | **Double**| The start time (including) to be queried. | [optional] [default to null]
 **toTime** | **Double**| The end time (excluding) to be queried. | [optional] [default to null]
 **skip** | **Integer**| The offset into the result. | [optional] [default to null]
 **top** | **Integer**| The maximum numbers of entries to be returned. | [optional] [default to null]

### Return type

[**Spikes**](..//Models/Spikes.md)

### Authorization

No authorization required

### HTTP request headers

- **Content-Type**: Not defined
- **Accept**: application/json

<a name="nestGetSpikesBySpikedetector"></a>
# **nestGetSpikesBySpikedetector**
> Spikes nestGetSpikesBySpikedetector(spikedetectorId, fromTime, toTime, nodeIds, skip, top)

Retrieves the spikes for the given time range (optional) and node IDs (optional) from one spike detector. If no time range or node list is specified, it will return the spikes for whole time or all nodes respectively.

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **spikedetectorId** | **Integer**| The ID of the spike detector to query. | [default to null]
 **fromTime** | **Double**| The start time in milliseconds (including) to be queried. | [optional] [default to null]
 **toTime** | **Double**| The end time in milliseconds (excluding) to be queried. | [optional] [default to null]
 **nodeIds** | [**List**](..//Models/Integer.md)| A list of node IDs queried for spike data. | [optional] [default to null]
 **skip** | **Integer**| The offset into the result. | [optional] [default to null]
 **top** | **Integer**| The maximum number of entries to be returned. | [optional] [default to null]

### Return type

[**Spikes**](..//Models/Spikes.md)

### Authorization

No authorization required

### HTTP request headers

- **Content-Type**: Not defined
- **Accept**: application/json

