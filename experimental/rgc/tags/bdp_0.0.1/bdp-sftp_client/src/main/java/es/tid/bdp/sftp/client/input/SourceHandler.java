package es.tid.bdp.sftp.client.input;

import java.io.IOException;
import java.io.OutputStream;
import java.util.List;

public interface SourceHandler {

    List<String> getFiles(String path, String regex);

    void copy(String inputPath, OutputStream output) throws IOException;

}
