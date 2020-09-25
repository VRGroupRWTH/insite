# coding: utf-8

from __future__ import absolute_import
import unittest

from flask import json
from six import BytesIO

from access_node.models.error_response import ErrorResponse  # noqa: E501
from access_node.models.version import Version  # noqa: E501
from access_node.test import BaseTestCase


class TestDefaultController(BaseTestCase):
    """DefaultController integration test stubs"""

    def test_get_version(self):
        """Test case for get_version

        Returns the deployed insite and API versions.
        """
        headers = { 
            'Accept': 'application/json',
        }
        response = self.client.open(
            '/v1/version',
            method='GET',
            headers=headers)
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))


if __name__ == '__main__':
    unittest.main()
