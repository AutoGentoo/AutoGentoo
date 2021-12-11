#!/usr/bin/env python3
import sys
import unittest
from typing import List, Optional

from autogentoo import network


SERVER_ADDR = "/tmp/network.autogentoo.test.sock"


class TestStringMethods(unittest.TestCase):
    def setUp(self) -> None:
        self.server = network.TCPServer(SERVER_ADDR)
        self.server.start()

    def tearDown(self) -> None:
        self.server.stop()

    def test_simple_message(self):
        s_message = network.build_message(0, 2, 3, 4, 5, 6)

        d: List[Optional[network.Message]] = [None]

        def cb(message: network.Message):
            d[0] = message
            return network.build_message(1, 4)

        self.server.set_request_callback(cb)
        response = network.send_message(SERVER_ADDR, s_message)

        self.assertEqual(d[0], s_message)
        self.assertEqual(response.token, 1)
        self.assertEqual(response.val1, int(4).to_bytes(8, byteorder=sys.byteorder))


if __name__ == "__main__":
    unittest.main()
