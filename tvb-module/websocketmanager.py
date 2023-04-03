import websocket
import struct
import threading
import orjson
import numpy as np
from enum import Enum

class InsiteOpcode(Enum):
    data = b'\x01'
    registerMonitor = b'\x02'
    startSim = b'\x03'
    endSim = b'\x04'
    deregisterMonitor = b'\x05'
    simInfo = b'\x06'
    simInfoReturn = b'\x07'


def serialize(attr): 
    if callable(getattr(attr,"summary_info",None)):
        return attr.summary_info()
    elif type(attr) == np.ndarray:
        return attr
    elif attr == None:
        return None
    elif type(attr) == list:
        return [serialize(attribute) for attribute in attr]
    elif type(attr) == dict:
        return { serialize(attribute_key): serialize(attribute_val) for attribute_key, attribute_val in attr.items()}
    elif type(attr) == set:
        return { serialize(attribute) for attribute in attr}
    else:
        try:
            orjson.dumps(attr)
            return attr;
        except: 
            return { "error": "type was: "+ str(type(attr))}


class WebSocketManager():
    def __init__(self, uri, port):
        self.sim_running = False
        self.uri = uri
        self.port = port
        self.sim = None
        self.websocket = websocket.WebSocketApp("ws://" + self.uri + ":" + str(self.port) + "/tvb",on_message=self.on_msg)
        self.binary_send_buffer = []


    def on_msg(self,wsapp,msg):
        splits = msg.split("/")

        attr = self.sim


        for split in splits:
            if type(attr) == list:
                try:
                    split = int(split)
                    if split >= 0 and split < len(attr):
                        attr = attr[split]
                    else:
                        attr = {"error" : "out of bounds"}
                        break;
                except:
                    attr = {"error" : "failed convert"}
                    break;
            else:
                if hasattr(attr,split):
                    attr = attr.__getattribute__(split)
                else:
                    attr = {"error" : "unknown attribute"}
                    break

        self.send_immediately(InsiteOpcode.simInfoReturn.value + orjson.dumps(serialize(attr),option=orjson.OPT_SERIALIZE_NUMPY))

    def connect(self):
        t = threading.Thread(target=self.websocket.run_forever)
        t.start() 

    def send_binary_buffered(self):
        for out in self.binary_send_buffer:
            out = out + self.binary_send_buffer.pop()
        self.send_binary_immediately(out)

    def add_to_binary_send_buffer(self, message):
        self.binary_send_buffer.append(message)

    def send_immediately(self, message):
        self.websocket.sock.send(message)

    def register_new_monitor(self, internalId, name, gid, observedVariables):
        json_metadata = {"internalId": internalId,"name": name, 
                               "gid" : gid, 
                               "observedVariables": observedVariables }
        message = InsiteOpcode.registerMonitor.value + orjson.dumps(json_metadata)
        self.websocket.sock.send_binary(message)

    def deregister_monitor(self, internalId, gid):
        message = InsiteOpcode.endSim.value + orjson.dumps({"internalId": internalId, "gid" : gid})
        self.websocket.sock.send_binary(message)

    def send_timestep(self, internalId, data):
        time  = data[0]
        shape = data[1].shape
        data_type = str.encode(data[1].dtype.char)
        self.websocket.sock.send_binary(InsiteOpcode.data.value + struct.pack("qdqqq8s",internalId,time,*shape, data_type) + data[1].tobytes())

    def send_binary_immediately(self, message):
        self.websocket.sock.send_binary(message)

    def send_numpy_array_immediately(self, arr : np.ndarray):
        self.websocket.sock.send_binary(arr.tobytes())
    
    def send_start_sim(self):
        if self.sim_running is False:
            self.websocket.sock.send_binary(InsiteOpcode.startSim.value)
        self.sim_running = True

    def send_end_sim(self):
        if self.sim_running:
            self.websocket.sock.send_binary(InsiteOpcode.endSim.value)
        self.sim_running = False

    def send_sim_info(self, sim):
        self.sim = sim
        self.send_binary_immediately(InsiteOpcode.simInfo.value + orjson.dumps(sim.summary_info()))

    def close(self):
        self.websocket.close()
