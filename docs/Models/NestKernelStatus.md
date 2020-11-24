# NestKernelStatus
## Properties

Name | Type | Description | Notes
------------ | ------------- | ------------- | -------------
**resolution** | [**BigDecimal**](number.md) | The resolution of the simulation (in ms) | [optional] [default to null]
**time** | [**BigDecimal**](number.md) | The current simulation time | [optional] [default to null]
**toUnderscoredo** | [**Integer**](integer.md) | The number of steps yet to be simulated (read only) | [optional] [default to null]
**maxUnderscoredelay** | [**BigDecimal**](number.md) | The maximum delay in the network | [optional] [default to null]
**minUnderscoredelay** | [**BigDecimal**](number.md) | The minimum delay in the network | [optional] [default to null]
**msUnderscoreperUnderscoretic** | [**BigDecimal**](number.md) | The number of milliseconds per tic | [optional] [default to null]
**ticsUnderscoreperUnderscorems** | [**BigDecimal**](number.md) | The number of tics per millisecond | [optional] [default to null]
**ticsUnderscoreperUnderscorestep** | [**Integer**](integer.md) | The number of tics per simulation time step | [optional] [default to null]
**TUnderscoremax** | [**BigDecimal**](number.md) | The largest representable time value (read only) | [optional] [default to null]
**TUnderscoremin** | [**BigDecimal**](number.md) | The smallest representable time value (read only) | [optional] [default to null]
**totalUnderscorenumUnderscorevirtualUnderscoreprocs** | [**Integer**](integer.md) | The total number of virtual processes | [optional] [default to null]
**localUnderscorenumUnderscorethreads** | [**Integer**](integer.md) | The local number of threads | [optional] [default to null]
**numUnderscoreprocesses** | [**Integer**](integer.md) | The number of MPI processes (read only) | [optional] [default to null]
**offUnderscoregridUnderscorespiking** | [**Boolean**](boolean.md) | Whether to transmit precise spike times in MPI communication (read only) | [optional] [default to null]
**initialUnderscoreconnectorUnderscorecapacity** | [**Integer**](integer.md) | When a connector is first created, it starts with this capacity (if &gt;&#x3D; connector_cutoff) | [optional] [default to null]
**largeUnderscoreconnectorUnderscorelimit** | [**Integer**](integer.md) | Capacity doubling is used up to this limit | [optional] [default to null]
**largeUnderscoreconnectorUnderscoregrowthUnderscorefactor** | [**BigDecimal**](number.md) | Capacity growth factor to use beyond the limit | [optional] [default to null]
**grngUnderscoreseed** | [**Integer**](integer.md) | Seed for global random number generator used synchronously by all virtual processes to create, e.g., fixed fan-out connections (write only). | [optional] [default to null]
**rngUnderscoreseeds** | [**List**](integer.md) | Seeds for the per-virtual-process random number generators used for most purposes. Array with one integer per virtual process, all must be unique and differ from grng_seed (write only). | [optional] [default to null]
**dataUnderscorepath** | [**String**](string.md) | A path, where all data is written to (default is the current directory) | [optional] [default to null]
**dataUnderscoreprefix** | [**String**](string.md) | A common prefix for all data files | [optional] [default to null]
**overwriteUnderscorefiles** | [**Boolean**](boolean.md) | Whether to overwrite existing data files | [optional] [default to null]
**printUnderscoretime** | [**Boolean**](boolean.md) | Whether to print progress information during the simulation | [optional] [default to null]
**networkUnderscoresize** | [**Integer**](integer.md) | The number of nodes in the network (read only) | [optional] [default to null]
**numUnderscoreconnections** | [**Integer**](integer.md) | The number of connections in the network (read only, local only) | [optional] [default to null]
**useUnderscorewfr** | [**Boolean**](boolean.md) | Whether to use waveform relaxation method | [optional] [default to null]
**wfrUnderscorecommUnderscoreinterval** | [**BigDecimal**](number.md) | Desired waveform relaxation communication interval | [optional] [default to null]
**wfrUnderscoretol** | [**BigDecimal**](number.md) | Convergence tolerance of waveform relaxation method | [optional] [default to null]
**wfrUnderscoremaxUnderscoreiterations** | [**Integer**](integer.md) | Maximal number of iterations used for waveform relaxation | [optional] [default to null]
**wfrUnderscoreinterpolationUnderscoreorder** | [**Integer**](integer.md) | Interpolation order of polynomial used in wfr iterations | [optional] [default to null]
**dictUnderscoremissUnderscoreisUnderscoreerror** | [**Boolean**](boolean.md) | Whether missed dictionary entries are treated as errors | [optional] [default to null]

[[Back to Model list]](../README.md#documentation-for-models) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to README]](../README.md)

