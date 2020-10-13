from typing import List

from access_node.models.spikedetector_info import SpikedetectorInfo  # noqa: E501

class SimulationNodes(object):
  def __init__(self, info_node: str=None, nest_simulation_nodes: List[str]=None, arbor_simulation_nodes: List[str]=None):
    self.nest_simulation_nodes = nest_simulation_nodes

simulation_nodes = SimulationNodes()
