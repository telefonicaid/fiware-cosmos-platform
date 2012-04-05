"""
Module bdp_fe.jobconf.tests

This module holds the unittests for the bdp_fe.jobconf app
"""
import os.path
import tempfile
import shutil

from django.core.files.uploadedfile import SimpleUploadedFile
from django.utils import unittest
from django.test.client import Client

from bdp_fe.jobconf import upload_util
from bdp_fe.jobconf.cluster import fakeserver, remote


class FileUtilTest(unittest.TestCase):

    def setUp(self):
        self.tmpdir = tempfile.mkdtemp()

    def test_ensure_dir(self):
        test_dir = os.path.join(self.tmpdir, 'test/dir/')
        self.assertFalse(os.path.isdir(test_dir))

        # Create dir when necessary
        upload_util.ensure_dir(test_dir)
        self.assertTrue(os.path.isdir(test_dir))

        # OK if the dir already exists
        upload_util.ensure_dir(test_dir)
        self.assertTrue(os.path.isdir(test_dir))

    def test_save(self):
        upload = SimpleUploadedFile.from_dict({
            'filename': 'upload',
            'content-type': 'application/json',
            'content' : '1',
        })
        target_dir = os.path.join(self.tmpdir, 'target')
        target_name = 'filename'

        upload_util.save(upload, target_dir, target_name)

        target_file = os.path.join(target_dir, target_name)
        self.assertTrue(os.path.isfile(target_file))
        with open(target_file) as f:
            self.assertEquals('1', f.read())

    def tearDown(self):
        shutil.rmtree(self.tmpdir)



class RemoteClusterTest(unittest.TestCase):

    def setUp(self):
        self.host = "localhost"
        self.port = 8282
        self.server = fakeserver.FakeServer(self.host, self.port)
        self.server.start()

    def test_copyToHdfs(self):
        cluster = remote.Cluster(self.host, self.port)
        cluster.copyToHdfs('/local/path', '/remote/path')
        self.assertEquals([['/local/path', '/remote/path']],
                          self.server.handler.copyToHdfsCalls)

    def tearDown(self):
        self.server.stop()
