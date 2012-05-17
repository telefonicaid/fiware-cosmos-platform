package es.tid.cosmos.platform.injection.server;

import java.io.IOException;
import java.net.URI;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.sshd.server.FileSystemView;
import org.apache.sshd.server.SshFile;

import es.tid.cosmos.base.util.Logger;

/**
 * HadoopFileSystemView
 *
 * @author logc
 * @since  CTP 2
 */
public class HadoopFileSystemView implements FileSystemView {
    private String homePath;
    private FileSystem hadoopFS;
    private final String userName;
    private final org.apache.log4j.Logger LOG =
            Logger.get(HadoopFileSystemView.class);

    public HadoopFileSystemView(String userName, Configuration configuration)
            throws IOException, InterruptedException {
        this.userName = userName;
        try {
            this.hadoopFS = FileSystem.get(
                    URI.create(configuration.get("fs.default.name")),
                    configuration, this.userName);
            this.homePath = this.hadoopFS.getHomeDirectory().toString()
                    .replaceFirst(this.hadoopFS.getUri().toString(), "");
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
        LOG.trace("view asked for file " + file);
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
        LOG.trace("view asked for ssh path " + baseDir.toString() +
                " and file " + file);
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
        LOG.trace("view asked for dir " + baseDir + " and file " + file);
        if (baseDir.isEmpty()){
            if (file.isEmpty()) {
            throw new IllegalArgumentException(
                    "filesystem view impossible for empty dir and filename!");
            } else if (file.equals(Path.CUR_DIR)) {
                baseDir = this.homePath;
                file = "";
                LOG.debug("redirecting to home path: " + this.homePath);
            }
        }
        String wholePath = baseDir + file;
        if (!baseDir.endsWith(Path.SEPARATOR) &&
                !file.startsWith(Path.SEPARATOR)) {
            wholePath = baseDir + Path.SEPARATOR + file;
        }
        LOG.trace("trying to get a view for path: " + wholePath);
        return new HadoopSshFile(wholePath, this.userName, this.hadoopFS);
    }
}
