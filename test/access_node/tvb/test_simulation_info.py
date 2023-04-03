import tvb_general_test_functions
from tvb_config import *
from numbers import Number

simulationInfoEndpoint = "/simulationInfo/"

def simulation_has_all_fields(info):
    assert("Type" in info), f"Simulation info member Type missing"
    assert("title" in info), f"Simulation info member title missing"
    assert("connectivity" in info), f"Simulation info member connectivity missing"
    assert("conduction_speed" in info), f"Simulation info member conduction_speed missing"
    assert("coupling" in info), f"Simulation info member coupling missing"
    assert("surface" in info), f"Simulation info member surface missing"
    assert("stimulus" in info), f"Simulation info member stimulus missing"
    assert("model" in info), f"Simulation info member model missing"
    assert("integrator" in info), f"Simulation info member integrator missing"
    assert("initial_conditions" in info), f"Simulation info member initial_conditions missing"
    assert("monitors" in info), f"Simulation toplevel info member monitors missing"
    assert("simulation_length" in info), f"Simulation info member simulation_length missing"
    assert("gid" in info), f"Simulation info gid missing"
    
def simulation_populated_all_fields(info):
    for member in info:
        assert(info[member] is not None), f"Simulation toplevel info has null member"
        
def simulation_populated_first_sublevel(subinfo):
    if type(subinfo) is dict:
        for member in subinfo:
            assert(not member.lower() == "error"), f"Simulation sublevel info has error member"
            assert(subinfo[member] is not None), f"Simulation sublevel info has null member"
            return
        
    if type(subinfo) is list:
        for member in subinfo:
            assert(member is not None), f"Simulation sublevel info has null member in list"
    
    assert(subinfo is not None), f"Simulation sublevel info is null"
    
def tests_get_simulation_info(tvb_simulation):
    simulation = tvb_general_test_functions.return_json_body_if_status_ok(BASE_REQUEST_URL + simulationInfoEndpoint)
    assert(simulation is not None)
    simulation_has_all_fields(simulation)
    simulation_populated_all_fields(simulation)
    
    for member in simulation:
        if member == "Type": continue                   
        if simulation[member] == "None": continue

        simulation_sublevel = tvb_general_test_functions.return_json_body_if_status_ok(BASE_REQUEST_URL + simulationInfoEndpoint + member)
        assert(simulation_sublevel is not None)
        assert(isinstance(simulation_sublevel,Number) or len(simulation_sublevel) > 0)
        simulation_populated_first_sublevel(simulation_sublevel)
    
def test_get_simulation_gid(tvb_simulation):
    tvb_general_test_functions.return_json_body_if_status_ok(BASE_REQUEST_URL + simulationInfoEndpoint + "gid")
