from django.test import TestCase

class AlwaysTrue(TestCase):
    def test_framework(self):
        '''This test case should always pass'''
        self.assert_(True)
