
import tvb.simulator.monitors as monitors
import time
from websocketmanager import WebSocketManager

class MonitorWrapper(monitors.Monitor):
    monitor_count = 0
    def __init__(self,wrapped_class : monitors.Monitor, websocket_manager : WebSocketManager , **kwargs):
        super(MonitorWrapper, self).__init__(**kwargs)
        self.websocket_manager = websocket_manager
        self.internalId = MonitorWrapper.monitor_count
        self.wgid = wrapped_class.gid
        self.monitor_type = wrapped_class.__class__.__name__
        MonitorWrapper.monitor_count = MonitorWrapper.monitor_count + 1
        time.sleep(0.01)
        self.wrapped_class = wrapped_class
        self.counter = 0

    def summary_info(self):
        return self.wrapped_class.summary_info()

    def __str__(self):
        return self.wrapped_class.__str__()
    
    def __del__(self):
        self.websocket_manager.deregister_monitor(self.internalId, self.wgid)

    def start_sim(self):
        self.websocket_manager.send_start_sim()
    
    def end_sim(self):
        self.websocket_manager.send_end_sim()

    def config_for_sim(self, simulator):
        self.wrapped_class.config_for_sim(simulator) 
        self.istep = self.wrapped_class.istep
        self.dt = self.wrapped_class.dt
        self.voi = self.wrapped_class.voi
        self.variables_of_interest = self.wrapped_class.variables_of_interest

        self.observed_vars = [simulator.model.variables_of_interest[index] for index in self.wrapped_class.voi ]
        self.websocket_manager.register_new_monitor(self.internalId, self.monitor_type, self.wgid, self.observed_vars)

    def sample(self, step, state):
        sample_data = self.wrapped_class.sample(step,state)
        if sample_data is not None:
            self.websocket_manager.send_timestep(self.internalId,sample_data)
            self.counter = self.counter+1

        return sample_data
