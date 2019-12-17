from typing import List

class Nodes(object):
  def __init__(self, info_node: str=None, simulation_nodes: List[str]=None):
    self.info_node = info_node
    self.simulation_nodes = simulation_nodes

nodes = Nodes()
