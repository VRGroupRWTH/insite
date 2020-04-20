from typing import List

class Nodes(object):
  def __init__(self, info_node: str=None, nest_simulation_nodes: List[str]=None, arbor_simulation_nodes: List[str]=None):
    self.info_node = info_node
    self.nest_simulation_nodes = nest_simulation_nodes
    self.arbor_simulation_nodes = arbor_simulation_nodes

nodes = Nodes()
