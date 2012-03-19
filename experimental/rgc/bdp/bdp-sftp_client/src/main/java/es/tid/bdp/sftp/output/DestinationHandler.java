package es.tid.bdp.sftp.output;

import java.io.IOException;
import java.io.OutputStream;

public interface DestinationHandler {

    OutputStream getOutputStream(String path) throws IOException;

}
