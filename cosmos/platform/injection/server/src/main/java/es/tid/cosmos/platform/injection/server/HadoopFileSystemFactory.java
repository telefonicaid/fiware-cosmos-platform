package es.tid.cosmos.platform.injection.server;

import java.io.IOException;

import org.apache.sshd.common.Session;
import org.apache.sshd.server.FileSystemFactory;

/**
 *
 * @author logc
 */
public class HadoopFileSystemFactory implements FileSystemFactory {
    @Override
    public HadoopFileSystemView createFileSystemView(Session session) throws IOException {
        // TODO: implement me!
        return null;
    }
}
