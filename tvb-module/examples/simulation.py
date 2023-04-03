import time
# from multiprocessing import Process, Pool, Queue
import orjson
import numpy as np
from tvb.simulator.lab import * 
from tvb.simulator.models.oscillator import Generic2dOscillator
from tvb.simulator.monitors import ProgressLogger
from enum import Enum
from websocketmanager import WebSocketManager, InsiteOpcode
from monitor_wrapper import MonitorWrapper
from os import environ


address = "localhost"
if environ.get('INSITE_ACCESS_NODE_URL') is not None:
    address = environ.get('INSITE_ACCESS_NODE_URL')
print(f"Connecting to: {address}")
insite_ws_manager = WebSocketManager(address,9011)
print(f"Connected to: {address}")

if __name__=="__main__":
    insite_ws_manager.connect()
    mon_raw = monitors.Raw()
    mon_spat = monitors.SpatialAverage()
    insite_spatial = MonitorWrapper(mon_spat,insite_ws_manager)
    insite_raw = MonitorWrapper(mon_raw,insite_ws_manager)

    length = 500
    #Bundle them
    what_to_watch = [insite_spatial]

    oscilator = Generic2dOscillator()
    oscilator.variables_of_interest = ("V","W","V + W")
    white_matter = connectivity.Connectivity.from_file()
    white_matter.speed = np.array([3.0])


    white_matter_coupling = coupling.Linear(a=np.array([0.00390625]))
    heunint = integrators.HeunDeterministic(dt=0.01220703125)



    insite_ws_manager.send_start_sim()
    sim = simulator.Simulator(model = oscilator, connectivity = white_matter,
                              coupling = white_matter_coupling, 
                              integrator = heunint, monitors = what_to_watch)
    insite_ws_manager.send_sim_info(sim)
    sim.configure()
    for data in sim(simulation_length=length):
        pass

    insite_ws_manager.send_end_sim()
    
    input()
    while True:
        time.sleep(5)
    insite_ws_manager.close()
