# Documentation for In-Situ Pipeline REST API

<a name="documentation-for-api-endpoints"></a>
## Documentation for API Endpoints

All URIs are relative to *http://localhost/v1*

Class | Method | HTTP request | Description
------------ | ------------- | ------------- | -------------
*DefaultApi* | [**getVersion**](Apis/DefaultApi.md#getversion) | **GET** /version | Returns the deployed insite and API versions.
*NestApi* | [**nestGetKernelStatus**](Apis/NestApi.md#nestgetkernelstatus) | **GET** /nest/kernelStatus | Retreives the current status of the NEST kernel.
*NestApi* | [**nestGetMultimeterById**](Apis/NestApi.md#nestgetmultimeterbyid) | **GET** /nest/multimeters/{multimeterId} | Retreives the available multimeters and their properties.
*NestApi* | [**nestGetMultimeterMeasurements**](Apis/NestApi.md#nestgetmultimetermeasurements) | **GET** /nest/multimeters/{multimeterId}/attributes/{attributeName} | Retrieves the measurements for a multimeter, attribute and node IDs (optional).
*NestApi* | [**nestGetMultimeters**](Apis/NestApi.md#nestgetmultimeters) | **GET** /nest/multimeters | Retreives the available multimeters and their properties.
*NestApi* | [**nestGetNodeById**](Apis/NestApi.md#nestgetnodebyid) | **GET** /nest/nodes/{nodeId} | Retrieves the properties of the specified node.
*NestApi* | [**nestGetNodeCollections**](Apis/NestApi.md#nestgetnodecollections) | **GET** /nest/nodeCollections | Retrieves a list of all node collection IDs.
*NestApi* | [**nestGetNodeIds**](Apis/NestApi.md#nestgetnodeids) | **GET** /nest/nodes/ids | Retrieves a list of all node IDs.
*NestApi* | [**nestGetNodeIdsByNodeCollection**](Apis/NestApi.md#nestgetnodeidsbynodecollection) | **GET** /nest/nodeCollections/${nodeCollectionId}/nodes/ids | Retrieves the list of all node ids within the node collection.
*NestApi* | [**nestGetNodes**](Apis/NestApi.md#nestgetnodes) | **GET** /nest/nodes | Retrieves all nodes of the simulation.
*NestApi* | [**nestGetNodesByNodeCollection**](Apis/NestApi.md#nestgetnodesbynodecollection) | **GET** /nest/nodeCollections/${nodeCollectionId}/nodes | Retrieves the list of all node within the node collection.
*NestApi* | [**nestGetSimulationTimeInfo**](Apis/NestApi.md#nestgetsimulationtimeinfo) | **GET** /nest/simulationTimeInfo | Retrieves simulation time information (stepSize, begin, current, end).
*NestApi* | [**nestGetSpikedetectors**](Apis/NestApi.md#nestgetspikedetectors) | **GET** /nest/spikedetectors/ | Queries all spike detectors accessable to the pipeline.
*NestApi* | [**nestGetSpikes**](Apis/NestApi.md#nestgetspikes) | **GET** /nest/spikes | Retrieves the spikes for the given time range (optional) and node IDs (optional). If no time range or node list is specified, it will return the spikes for whole time or all nodes respectively. This request merges the spikes recorded by all spike detectors and removes duplicates.
*NestApi* | [**nestGetSpikesByNodeCollection**](Apis/NestApi.md#nestgetspikesbynodecollection) | **GET** /nest/nodeCollections/{nodeCollectionId}/spikes | Retrieves the spikes for the given simulation steps (optional) and node collection. This request merges the spikes recorded by all spike detectors and removes duplicates.
*NestApi* | [**nestGetSpikesBySpikedetector**](Apis/NestApi.md#nestgetspikesbyspikedetector) | **GET** /nest/spikedetectors/{spikedetectorId}/spikes | Retrieves the spikes for the given time range (optional) and node IDs (optional) from one spike detector. If no time range or node list is specified, it will return the spikes for whole time or all nodes respectively.


<a name="documentation-for-models"></a>
## Documentation for Models

 - [Error](.//Models/Error.md)
 - [ErrorResponse](.//Models/ErrorResponse.md)
 - [InnerError](.//Models/InnerError.md)
 - [MultimeterInfo](.//Models/MultimeterInfo.md)
 - [MultimeterMeasurement](.//Models/MultimeterMeasurement.md)
 - [NestKernelStatus](.//Models/NestKernelStatus.md)
 - [NestNodeCollectionNodes](.//Models/NestNodeCollectionNodes.md)
 - [NestNodeCollectionProperties](.//Models/NestNodeCollectionProperties.md)
 - [NestNodeModel](.//Models/NestNodeModel.md)
 - [NestNodeProperties](.//Models/NestNodeProperties.md)
 - [SimulationTimeInfo](.//Models/SimulationTimeInfo.md)
 - [SpikedetectorInfo](.//Models/SpikedetectorInfo.md)
 - [Spikes](.//Models/Spikes.md)
 - [Version](.//Models/Version.md)


<a name="documentation-for-authorization"></a>
## Documentation for Authorization

All endpoints do not require authorization.
