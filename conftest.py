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
        process.wait()
        logfile.close()
    request.addfinalizer(finalize)

    while True:
        time.sleep(1.0)
        try:
            r = requests.get("http://localhost:8080/simulation_time_info")
            simulation_time_info = r.json()
            new_time = simulation_time_info['current']
            if new_time > 0.0:
                break
        except requests.exceptions.ConnectionError:
            pass

    return process