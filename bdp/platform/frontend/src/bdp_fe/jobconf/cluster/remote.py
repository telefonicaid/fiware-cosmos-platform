"""
Abstraction over the RPC interface to the cluster.

"""
import os.path
import sys
sys.path.append(os.path.join(os.path.dirname(__file__), 'gen-py'))

from cluster_api.Cluster import Client
from cluster_api.constants import *
from cluster_api.ttypes import TransferException

from thrift import Thrift
from thrift.transport import TSocket
from thrift.transport import TTransport
from thrift.protocol import TBinaryProtocol


class ConnException(Exception):
    pass


class ClusterException(Exception):

    def __init__(self, error_code, error_message):
        self.error_code = error_code
        self.error_message = error_message


def wrap_exceptions(fn):

    def wrapped_call(*args, **kwargs):
        try:
            return fn(*args, **kwargs)
        except TTransport.TTransportException, ex:
            trace = sys.exc_info()[2]
            raise ConnException("Connection problem"), None, trace
        except TransferException, ex:
            trace = sys.exc_info()[2]
            raise ClusterException(ex.errorCode, ex.errorMsg), None, trace

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
    def getJobResult(self, jobId):
        self.transport.open()
        result = self.cluster.getJobResult(jobId)
        self.transport.close()
        return result
