import pytest
import subprocess
import time
import signal
import requests 

@pytest.fixture(scope="module")
def nest_simulation(request):
    logfile = open("docker-compose.log", "w")
    process = subprocess.Popen(["docker-compose", "up"], stdout=logfile, stderr=subprocess.STDOUT)

    def finalize():
        process.send_signal(signal.SIGINT)
        process.wait()
        logfile.close()
    request.addfinalizer(finalize)
    time.sleep(10)
    return process

def test_stuff(nest_simulation):
    current_time = 0.0
    while current_time < 100:
        r = requests.get("http://localhost:8080/simulation_time_info")
        time = r.json()
        new_time = time['current']
        assert(new_time >= current_time)
        current_time = new_time
