"""
Fake cluster API server for testing purposes.

"""
import logging
import os.path
import socket
import sys
import threading
sys.path.append(os.path.join(os.path.dirname(__file__), 'gen-py'))

from cluster_api.Cluster import Processor
from cluster_api.constants import *

from thrift import Thrift
from thrift.transport import TSocket
from thrift.transport import TTransport
from thrift.protocol import TBinaryProtocol
from thrift.server import TServer


LOGGER = logging.getLogger(__name__)


class ClusterHandler:

    def __init__(self, *args):
        self.copyToHdfsCalls = []
        self.runJobCalls = []
        self.job_result = ClusterJobResult(status=ClusterJobStatus.SUCCESSFUL,
                                           reason=None)

    def copyToHdfs(self, src_path, dest_path):
        self.copyToHdfsCalls.append([src_path, dest_path])
        LOGGER.info("Fake copy from %s to %s" % (src_path, dest_path))

    def copyToHdfs(self, src_path, dest_path):
         self.copyToHdfsCalls.append([src_path, dest_path])
         LOGGER.info("Fake copy from %s to %s" % (src_path, dest_path))

    def runJob(self, job_id, jarPath, inputPath, outputPath, mongoUrl):
        self.runJobCalls.append([job_id, jarPath, inputPath, outputPath,
                                 mongoUrl])
        LOGGER.info("Fake run job %s" % job_id)

    def getJobResult(self, job_id):
        return self.job_result


class FakeServer(threading.Thread, TServer.TSimpleServer):
    """
    Fake server whose only purpose is logging API calls.
    Extends Thread to be able to run in the background.

    """

    def __init__(self, host, port):
        super(FakeServer, self).__init__()
        # Old-style super-constructor invocation
        self.handler = ClusterHandler()
        TServer.TSimpleServer.__init__(self,
            Processor(self.handler),
            TSocket.TServerSocket(host, port),
            TTransport.TBufferedTransportFactory(),
            TBinaryProtocol.TBinaryProtocolFactory())
        self.stopEvent = threading.Event()

    def run(self):
        self.serverTransport.listen()
        self.serverTransport.handle.settimeout(1000)
        while not self.stopEvent.is_set():
            client = self.serverTransport.accept()
            itrans = self.inputTransportFactory.getTransport(client)
            otrans = self.outputTransportFactory.getTransport(client)
            iprot = self.inputProtocolFactory.getProtocol(itrans)
            oprot = self.outputProtocolFactory.getProtocol(otrans)
            try:
                while True:
                    self.processor.process(iprot, oprot)
            except TTransport.TTransportException, tx:
                pass
            except Exception:
                break

        itrans.close()
        otrans.close()

    def stop(self):
        self.stopEvent.set()
        self.join()
