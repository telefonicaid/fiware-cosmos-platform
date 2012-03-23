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
    
    def __init__(self):
        pass

    def copyToHdfs(self, src_path, dest_path):
        LOGGER.info("Fake copy from %s to %s" % (src_path, dest_path))

class FakeServer(TServer.TSimpleServer):
    """
    Fake server whose only purpose is logging API calls.

    """

    def __init__(self, host, port):
        socket = TSocket.TServerSocket(host, port)
        # Old-style super-constructor invocation
        TServer.TSimpleServer.__init__(self,
            Processor(ClusterHandler()),
            socket,
            TTransport.TBufferedTransportFactory(),
            TBinaryProtocol.TBinaryProtocolFactory())
        self.stopEvent = threading.Event()

    def serve(self):
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
            except Exception, x:
                logging.exception(x)

        itrans.close()
        otrans.close()

    def stop(self):
        self.stopEvent.set()

class BackgroundFakeServer(threading.Thread):
    """
    Fake server running in a background thread.

    """

    def __init__(self, host, port):
        super(BackgroundFakeServer, self).__init__()
        self.server = FakeServer(host, port)

    def run(self):
        self.server.serve()

    def stop(self):
        self.server.stop()
        self.join()
