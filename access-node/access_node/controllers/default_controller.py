import connexion
import six

from access_node.models.error_response import ErrorResponse  # noqa: E501
from access_node.models.version import Version  # noqa: E501
from access_node import util


def get_version():  # noqa: E501
    """Returns the deployed insite and API versions.

     # noqa: E501


    :rtype: Version
    """
    return 'do some magic!'
