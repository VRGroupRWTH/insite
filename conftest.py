import pytest
import subprocess
import signal
import requests
import time
import os

@pytest.fixture(scope="session")
def nest_simulation(request):
    logfile = open("docker-compose.log", "w")

    subprocess.run(["docker-compose", "build"], stdout=logfile, stderr=subprocess.STDOUT)
    process = subprocess.Popen(["docker-compose", "up"], stdout=logfile, stderr=subprocess.STDOUT)

    def finalize():
        # process.send_signal(signal.SIGINT)
        os.kill(process.pid, signal.SIGINT)
        try:
            process.wait(10)
        except subprocess.TimeoutExpired:
            print("Sending SIGKILL to docker-compose")
            os.kill(process.pid, signal.SIGKILL)
            process.wait()
        subprocess.run(["docker-compose", "down"], stdout=logfile, stderr=subprocess.STDOUT)
        logfile.close()
    request.addfinalizer(finalize)

    begin_time = time.time()
    while True:
        time.sleep(1.0)
        try:
            r = requests.get("http://localhost:9000/version")
            if r.status_code == 200:
                break
        except requests.exceptions.ConnectionError:
            pass
        current_time = time.time()
        if current_time - begin_time > 60:
            pytest.fail('Timeout during container start')

    while True:
        time.sleep(1.0)
        try:
            r = requests.get("http://localhost:8080/version")
            if r.status_code == 200:
                break
        except requests.exceptions.ConnectionError:
            pass
        current_time = time.time()
        if current_time - begin_time > 60:
            pytest.fail('Timeout during container start')

    return process
