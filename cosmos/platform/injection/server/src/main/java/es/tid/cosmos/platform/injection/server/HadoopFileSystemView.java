package es.tid.cosmos.platform.injection.server;

import org.apache.sshd.server.FileSystemView;
import org.apache.sshd.server.SshFile;

/**
 *
 * @author logc
 */
public class HadoopFileSystemView implements FileSystemView {
    @Override
    public HadoopSshFile getFile(String file) {
        // TODO: implment me!
        return null;
    }

    @Override
    public HadoopSshFile getFile(SshFile baseDir, String file) {
        // TODO: implement me!
        return null;
    }
}
