import pytest
import subprocess
import signal
import requests
import time
import os

@pytest.fixture(scope="session")
def tvb_simulation(request):
    time.sleep(1)
    return True

@pytest.fixture(scope="session")
def nest_simulation(request):
    logfile = open("insite.log", "w")
    logfile_access_node = open("access_node.log", "w")
    def finalize():
        logfile.close()
    print("\nWaiting for NEST start up",end='')
    request.addfinalizer(finalize)

    print("Waiting for access node start up",end='')
    while True:
        print(".",end='',flush=True)
        time.sleep(1.0)
        try:
            r = requests.get("http://insite-access-node:52056/version")
            if r.status_code == 200:
                print("started.")
                break
        except requests.exceptions.ConnectionError:
            pass
        current_time = time.time()
        if current_time - begin_time > 60:
            pytest.fail('Timeout during container start')

    return True
