#!/usr/bin/env python3
import os
import sys
import unittest
sys.path.append(os.path.join(os.path.dirname(__file__), "../"))
import cportage


class TestStringMethods(unittest.TestCase):
    def test_simple_message(self):
        pass


if __name__ == '__main__':
    unittest.main()
