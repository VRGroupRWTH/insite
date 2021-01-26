import requests
import numbers


def test_get_kernel_status(nest_simulation):
    request = requests.get("http://localhost:8080/nest/kernelStatus")
    assert (request.status_code == 200)

    kernel_statuses = request.json()
    assert (isinstance(kernel_statuses, list))

    for kernel_status in kernel_statuses:
        assert (isinstance(kernel_status, dict))

        assert ('num_processes' in kernel_status)
        assert (isinstance(kernel_status['num_processes'], numbers.Number))


        assert ('local_num_threads' in kernel_status)
        assert (kernel_status['local_num_threads'] == 1)
        assert (isinstance(kernel_status['local_num_threads'],numbers.Number))
