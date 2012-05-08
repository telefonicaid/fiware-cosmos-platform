package es.tid.cosmos.platform.injection.server;

import java.io.IOException;

import org.apache.sshd.server.FileSystemView;
import org.apache.sshd.server.SshFile;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 *
 * @author logc
 */
public class HadoopFileSystemView implements FileSystemView {
    private final Logger LOG;
    private String userName;

    public HadoopFileSystemView(String userName) {
        this.userName = userName;
        this.LOG = LoggerFactory.getLogger(HadoopFileSystemView.class);
    }

    @Override
    public HadoopSshFile getFile(String file) {
//        throw new UnsupportedOperationException(
//                String.format("method getFile not yet implemented; asked for %s",
//                        file));
        // TODO: determine system root
        if (file.equals(".")) {
            file = "/";
        }
        // TODO: get real userId
        try {
            return new HadoopSshFile(file, userName, 1);
        } catch (IOException e) {
            LOG.error(e.getLocalizedMessage());
            return null;
        }
    }

    @Override
    public HadoopSshFile getFile(SshFile baseDir, String file) {
        throw new UnsupportedOperationException(
                "method getFile not yet implemented");
    }

    private HadoopSshFile getFile(String baseDir, String file) {
        try {
            return new HadoopSshFile("lacabra", "user", 1);
        } catch (IOException e) {
            return null;
        }
    }
}
