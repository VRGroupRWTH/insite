import requests
import numbers

def test_get_kernel_status(nest_simulation):
    request = requests.get("http://localhost:18080/kernelStatus")
    assert(request.status_code == 200)
    
    kernel_status = request.json()


    assert(isinstance(kernel_status, dict))

    assert('num_processes' in kernel_status)
    assert(isinstance(kernel_status['num_processes'],numbers.Number))
        
    assert('local_num_threads' in kernel_status)
    assert(isinstance(kernel_status['local_num_threads'],numbers.Number))
