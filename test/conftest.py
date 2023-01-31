import pytest
import subprocess
import signal
import requests
import time
import os

@pytest.fixture(scope="session")
def nest_simulation(request):
    logfile = open("docker-compose.log", "w")
    logfile_access_node = open("access_node.log", "w")

    print("Starting docker-compose build")
    # subprocess.run(["docker-compose", "build"], stdout=logfile, stderr=subprocess.STDOUT)
    print("Finished docker-compose build")
    print("Starting docker-compose")
    # process = subprocess.Popen(["docker-compose", "-f", "docker-compose-testing.yml" ,"up"], stdout=logfile, stderr=subprocess.STDOUT)
    process = subprocess.Popen(["python","./nest-module/example/pytest_simulation.py"], stdout=logfile, stderr=subprocess.STDOUT)
    process2 = subprocess.Popen(["./insite-access-node"], stdout=logfile_access_node, stderr=subprocess.STDOUT)
    time.sleep(1)

    def finalize():
        # process.send_signal(signal.SIGINT)
        process.send_signal(signal.SIGINT)
        process2.send_signal(signal.SIGINT)
        # os.kill(process.pid, signal.SIGINT)
        # os.kill(process2.pid, signal.SIGINT)
        try:
            process.wait(10)
        except subprocess.TimeoutExpired:
            print("\nSending SIGKILL to docker-compose")
            # os.kill(process.pid, signal.SIGKILL)
            # os.kill(process2.pid, signal.SIGKILL)
            process.kill()
            process2.kill()
            process.wait()
            process2.wait()
        # subprocess.run(["docker-compose", "down"], stdout=logfile, stderr=subprocess.STDOUT)
        logfile.close()
    request.addfinalizer(finalize)

    begin_time = time.time()
    print("\nWaiting for insite start up",end='')
    while True:
        print(".",end='',flush=True)
        time.sleep(1.0)
        try:
            r = requests.get("http://localhost:18080/version")
            if r.status_code == 200:
                print("started.")
                break
        except requests.exceptions.ConnectionError:
            pass
        current_time = time.time()
        if current_time - begin_time > 60:
            pytest.fail('Timeout during container start')

    print("Waiting for access node start up",end='')
    while True:
        print(".",end='',flush=True)
        time.sleep(1.0)
        try:
            r = requests.get("http://localhost:8080/version")
            if r.status_code == 200:
                print("started.")
                break
        except requests.exceptions.ConnectionError:
            pass
        current_time = time.time()
        if current_time - begin_time > 60:
            pytest.fail('Timeout during container start')

    return process
