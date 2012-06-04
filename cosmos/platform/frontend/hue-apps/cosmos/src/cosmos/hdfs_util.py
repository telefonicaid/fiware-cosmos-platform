# -*- coding: utf-8 -*-
"""
HDFS utilities.
"""
import os.path
import tempfile as tmp
from shutil import rmtree


class TemporalDirectory(object):
    """
    Temporal directory with automatic naming whose lifespan equals the object
    one. Intended for use within the 'with' sentence.
    """

    def __init__(self, prefix='tmp'):
        self.path = tmp.mkdtemp(prefix=prefix)

    def child_path(self, filename):
        return os.path.join(self.path, filename)

    def close(self):
        rmtree(self.path)


class CachedHDFSFile(object):
    """
    Allows to operate with a HDFS file locally through a cached file.
    Intended to be used within 'with' construct.
    """

    CHUNK_SIZE = 4096

    def __init__(self, fs, path):
        self.tmp_dir = TemporalDirectory(prefix='hdfscache')
        self.cached_file = os.path.join(self.tmp_dir.path,
                                        os.path.basename(path))
        with fs.open(path, 'r') as hdfs_file:
            with open(self.cached_file, 'w') as local_file:
                for chunk in iter((lambda: hdfs_file.read(self.CHUNK_SIZE)), ''):
                    local_file.write(chunk)

    def local_path(self):
        return self.cached_file

    def close(self):
        self.tmp_dir.close()
