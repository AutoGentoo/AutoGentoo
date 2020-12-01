import unittest

import network


class TestStringMethods(unittest.TestCase):
    def setUp(self) -> None:
        self.server = network.AutoGentooTCPServer("/tmp/network.autogentoo.test.sock")
        self.server.start()

    def tearDown(self) -> None:
        self.server.stop()

    def test_simple_message(self):
        pass


if __name__ == '__main__':
    unittest.main()
