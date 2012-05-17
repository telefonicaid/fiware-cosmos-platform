package es.tid.cosmos.platform.injection.server;

import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.sshd.common.Session;
import org.apache.sshd.server.FileSystemFactory;
import org.apache.sshd.server.FileSystemView;

/**
 * HadoopFileSystemFactory
 *
 * @author logc
 * @since  CTP 2
 */
public class HadoopFileSystemFactory implements FileSystemFactory {
    private final Configuration configuration;

    public HadoopFileSystemFactory(Configuration configuration) {
        this.configuration = configuration;
    }

    @Override
    public FileSystemView createFileSystemView(Session session)
            throws IOException {
        return new HadoopFileSystemView(session.getUsername(),
                this.configuration);
    }
}
