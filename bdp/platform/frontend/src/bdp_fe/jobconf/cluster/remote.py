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


class ConnException(Exception):
    pass


def wrap_exceptions(fn):

    def wrapped_call(*args, **kwargs):
        try:
            return fn(*args, **kwargs)
        except TTransport.TTransportException, ex:
            trace = sys.exc_info()[2]
            raise ConnException("Connection problem"), None, trace

    return wrapped_call


class Cluster:

    def __init__(self, host, port):
        self.transport = TTransport.TBufferedTransport(
            TSocket.TSocket(host, port))
        protocol = TBinaryProtocol.TBinaryProtocol(self.transport)
        self.cluster = Client(protocol)

    @wrap_exceptions
    def copyToHdfs(self, src_path, dest_path):
        self.transport.open()
        self.cluster.copyToHdfs(src_path, dest_path)
        self.transport.close()

    @wrap_exceptions
    def runJob(self, jarPath, inputPath, outputPath, mongoUrl):
        self.transport.open()
        execution_id = self.cluster.runJob(jarPath, inputPath, outputPath, mongoUrl)
        self.transport.close()
        return execution_id

    @wrap_exceptions
    def getJobStatus(self, jobId):
        self.transport.open()
        status = self.cluster.getJobStatus(jobId)
        self.transport.close()
        return status
