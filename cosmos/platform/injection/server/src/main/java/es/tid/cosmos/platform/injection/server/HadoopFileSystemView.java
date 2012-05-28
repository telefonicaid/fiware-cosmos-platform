package es.tid.cosmos.platform.injection.server;

import java.io.IOException;
import java.net.URI;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.sshd.server.FileSystemView;
import org.apache.sshd.server.SshFile;

import es.tid.cosmos.base.data.MessageDescriptor;
import es.tid.cosmos.base.util.Logger;

/**
 * HadoopFileSystemView
 *
 * @author logc
 * @since  CTP 2
 */
public class HadoopFileSystemView implements FileSystemView {
    private static final org.apache.log4j.Logger LOG =
            Logger.get(HadoopFileSystemView.class);
    private static final String FORMAT_FILE = "format.properties";

    private String homePath;
    private FileSystem hadoopFS;
    private final String userName;
    private MessageDescriptor descriptor;

    public HadoopFileSystemView(String userName, Configuration configuration)
            throws IOException, InterruptedException {
        this.userName = userName;
        try {
            this.hadoopFS = FileSystem.get(
                    URI.create(configuration.get("fs.default.name")),
                    configuration, this.userName);
            this.homePath = this.hadoopFS.getHomeDirectory().toString()
                    .replaceFirst(this.hadoopFS.getUri().toString(), "");
            HadoopSshFile descriptorFile = this.getFile(
                    this.hadoopFS.getHomeDirectory().toString(), FORMAT_FILE);
            if (descriptorFile.doesExist()) {
                this.descriptor = new MessageDescriptor(
                        descriptorFile.createInputStream(0L));
            }
        } catch (IOException e) {
            LOG.error(e.getMessage(), e);
            throw e;
        } catch (InterruptedException e) {
            LOG.error(e.getMessage(), e);
            throw e;
        }
    }

    @Override
    public HadoopSshFile getFile(String file) {
        try {
            return this.getFile("", file);
        } catch (IOException e) {
            LOG.error(e.getMessage(), e);
        } catch (InterruptedException e) {
            LOG.error(e.getMessage(), e);
        }
        return null;
    }

    @Override
    public HadoopSshFile getFile(SshFile baseDir, String file) {
        try {
            return this.getFile(baseDir.getAbsolutePath(), file);
        } catch (IOException e) {
            LOG.error(e.getMessage(), e);
        } catch (InterruptedException e) {
            LOG.error(e.getMessage(), e);
        }
        return null;
    }

    private HadoopSshFile getFile(String baseDir, String file)
            throws IOException, InterruptedException {
        String requestedDir = baseDir;
        String requestedFile = file;
        if (requestedDir.isEmpty() && (requestedFile.isEmpty() ||
                requestedFile.equals(Path.CUR_DIR))) {
            requestedDir = this.homePath;
            requestedFile = "";
            LOG.debug("redirecting to home path: " + this.homePath);
        }
        String wholePath = requestedDir + requestedFile;
        if (!requestedDir.endsWith(Path.SEPARATOR) &&
                !requestedFile.startsWith(Path.SEPARATOR)) {
            wholePath = requestedDir + Path.SEPARATOR + requestedFile;
        }
        return new HadoopSshFile(wholePath, this.userName,
                                 this.hadoopFS, this.descriptor);
    }
}
