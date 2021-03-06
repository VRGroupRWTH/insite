# coding: utf-8

from __future__ import absolute_import
from datetime import date, datetime  # noqa: F401

from typing import List, Dict  # noqa: F401

from access_node.models.base_model_ import Model
from access_node import util


class InnerError(Model):
    """NOTE: This class is auto generated by OpenAPI Generator (https://openapi-generator.tech).

    Do not edit the class manually.
    """

    def __init__(self, code=None, innererror=None):  # noqa: E501
        """InnerError - a model defined in OpenAPI

        :param code: The code of this InnerError.  # noqa: E501
        :type code: str
        :param innererror: The innererror of this InnerError.  # noqa: E501
        :type innererror: InnerError
        """
        self.openapi_types = {
            'code': str,
            'innererror': InnerError
        }

        self.attribute_map = {
            'code': 'code',
            'innererror': 'innererror'
        }

        self._code = code
        self._innererror = innererror

    @classmethod
    def from_dict(cls, dikt) -> 'InnerError':
        """Returns the dict as a model

        :param dikt: A dict.
        :type: dict
        :return: The InnerError of this InnerError.  # noqa: E501
        :rtype: InnerError
        """
        return util.deserialize_model(dikt, cls)

    @property
    def code(self):
        """Gets the code of this InnerError.

        One of the error codes defined here: [...]  # noqa: E501

        :return: The code of this InnerError.
        :rtype: str
        """
        return self._code

    @code.setter
    def code(self, code):
        """Sets the code of this InnerError.

        One of the error codes defined here: [...]  # noqa: E501

        :param code: The code of this InnerError.
        :type code: str
        """
        if code is None:
            raise ValueError("Invalid value for `code`, must not be `None`")  # noqa: E501

        self._code = code

    @property
    def innererror(self):
        """Gets the innererror of this InnerError.


        :return: The innererror of this InnerError.
        :rtype: InnerError
        """
        return self._innererror

    @innererror.setter
    def innererror(self, innererror):
        """Sets the innererror of this InnerError.


        :param innererror: The innererror of this InnerError.
        :type innererror: InnerError
        """

        self._innererror = innererror
