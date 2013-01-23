# -*- coding: utf-8 -*-
"""
Tests of HDFS utilities.

"""
from contextlib import closing
import os
import os.path
from StringIO import StringIO
import unittest as test

from cosmos.hdfs_util import CachedHDFSFile, TemporalDirectory


def parent_dir(path):
    return os.path.abspath(os.path.join(path, os.path.pardir))


class TemporalDirectoryTestCase(test.TestCase):

    def test_create_and_remove_dir(self):
        with closing(TemporalDirectory()) as instance:
            path = instance.path
            self.assertTrue(os.path.isdir(path))
        self.assertFalse(os.path.isdir(path))

    def test_dir_naming(self):
        with closing(TemporalDirectory(prefix='prefix')) as instance:
            rel_path = os.path.relpath(instance.path, parent_dir(instance.path))
            self.assertTrue(rel_path.startswith('prefix'))

    def test_child_paths(self):
        with closing(TemporalDirectory()) as instance:
            self.assertEquals(instance.path,
                            parent_dir(instance.child_path('file0')))


class FakeHDFS(object):

    def __init__(self, files={}):
        self.files = files

    def open(self, path, mode):
        return StringIO(self.files[path])


class CachedHDFSFileTestCase(test.TestCase):

    def setUp(self):
        self.fake_fs = FakeHDFS(files={
            '/dir/file': "File contents"
        })

    def test_cache_hdfs_file(self):
        with closing(CachedHDFSFile(self.fake_fs, '/dir/file')) as instance:
            path = instance.local_path()
            self.assertTrue(os.path.isfile(path))
            with open(path, 'r') as f:
                self.assertEquals(f.read(), "File contents")
        self.assertFalse(os.path.isfile(path))

    def test_io_error(self):
        self.assertRaises(Exception, CachedHDFSFile, [self.fake_fs,
                                                      ':invalid;:file\\/name'])
