import connexion
import six

from access_node.models.error import Error  # noqa: E501
from access_node.models.error_response import ErrorResponse  # noqa: E501
from access_node.models.version import Version  # noqa: E501
from access_node import util

import os
import json

def get_version():  # noqa: E501
    """Returns the deployed insite and API versions.

     # noqa: E501


    :rtype: Version
    """
    dirname = os.path.dirname(__file__)
    filename = os.path.join(dirname, '../../version.txt')
    print(filename)
    with open(filename, "r") as json_file:
        versions = json.load(json_file)
        api_version = versions["api_version"]
        insite_version = versions["insite_version"]
        version = Version(api=api_version,insite=insite_version)
        return version, 200
    error = Error(code ="NoVersion", message = "Unable to read version from file")
    error_response = ErrorResponse(error)
    return error_response, 500
