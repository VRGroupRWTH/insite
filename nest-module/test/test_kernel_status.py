import requests
import numbers

def test_get_kernel_status(nest_simulation):
    request = requests.get("http://localhost:9000/kernelStatus")
    assert(request.status_code == 200)
    
    kernel_status = request.json()

    assert(isinstance(kernel_status, dict))

    assert('num_processes' in kernel_status)
    assert(kernel_status['num_processes'] == 1)
    
    assert('local_num_threads' in kernel_status)
    assert(kernel_status['local_num_threads'] == 1)
