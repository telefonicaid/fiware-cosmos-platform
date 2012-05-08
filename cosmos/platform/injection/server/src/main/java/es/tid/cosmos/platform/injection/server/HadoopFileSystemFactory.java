package es.tid.cosmos.platform.injection.server;

import java.io.IOException;

import org.apache.sshd.common.Session;
import org.apache.sshd.server.FileSystemFactory;
import org.apache.sshd.server.FileSystemView;

/**
 *
 * @author logc
 */
public class HadoopFileSystemFactory implements FileSystemFactory {
    @Override
    public FileSystemView createFileSystemView(Session session)
            throws IOException {
        return new HadoopFileSystemView(session.getUsername());
    }
}
