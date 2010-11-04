#! /usr/bin/env python

class rat:
        def __init__(self, n, d):
                n/d
                self._n, self._d = n, d
        def prnt(self):
                print "{0}/{1}".format(
						self._n, self._d)


