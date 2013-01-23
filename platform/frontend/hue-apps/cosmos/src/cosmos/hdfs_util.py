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
    one. Intended to be used within 'with' construct.
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
        hdfs_file = None
        local_file = None
        try:
            hdfs_file = fs.open(path, 'r')
            local_file = open(self.cached_file, 'w')
            for chunk in iter((lambda: hdfs_file.read(self.CHUNK_SIZE)), ''):
                local_file.write(chunk)
        finally:
            if hdfs_file:
                hdfs_file.close()
            if local_file:
                local_file.close()

    def local_path(self):
        return self.cached_file

    def close(self):
        self.tmp_dir.close()
