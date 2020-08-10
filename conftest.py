import pytest
import subprocess
import signal
import requests
import time

@pytest.fixture(scope="session")
def nest_simulation(request):
    logfile = open("docker-compose.log", "w")
    process = subprocess.Popen(["docker-compose", "up", "--build"], stdout=logfile, stderr=subprocess.STDOUT)

    def finalize():
        process.send_signal(signal.SIGINT)
        try:
            process.wait(5)
        except subprocess.TimeoutExpired:
            process.send_signal(signal.SIGKILL)
            process.wait()
        logfile.close()
    request.addfinalizer(finalize)

    while True:
        time.sleep(1.0)
        try:
            r = requests.get("http://localhost:8080/nest/simulation_time_info")
            if r.status_code == 200:
                break
        except requests.exceptions.ConnectionError:
            pass

    return process