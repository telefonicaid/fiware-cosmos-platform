"""
Abstraction over the RPC interface to the cluster.

"""
import os.path
import sys
sys.path.append(os.path.join(os.path.dirname(__file__), 'gen-py'))

from cluster_api.Cluster import Client
from cluster_api.constants import *

from thrift import Thrift
from thrift.transport import TSocket
from thrift.transport import TTransport
from thrift.protocol import TBinaryProtocol


class Cluster:

    def __init__(self, host, port):
        self.transport = TTransport.TBufferedTransport(
            TSocket.TSocket(host, port))
        protocol = TBinaryProtocol.TBinaryProtocol(self.transport)
        self.cluster = Client(protocol)

    def copyToHdfs(self, src_path, dest_path):
        self.transport.open()
        self.cluster.copyToHdfs(src_path, dest_path)
        self.transport.close()

    def runJob(self, jarPath, inputPath, outputPath, mongoUrl):
        self.transport.open()
        self.cluster.runJob(jarPath, inputPath, outputPath, mongoUrl)
        self.transport.close()

    def close(self):
        self.transport.close()
