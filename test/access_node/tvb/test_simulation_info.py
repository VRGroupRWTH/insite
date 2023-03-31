import tvb_general_test_functions
from tvb_config import *


def tests_get_simulation_info():
    tvb_general_test_functions.return_json_body_if_status_ok(BASE_REQUEST_URL + "/simulation_info/")

# def test_get_simulation_gid():
#     tvb_general_test_functions.return_json_body_if_status_ok(BASE_REQUEST_URL + "/simulation_info/gid")
