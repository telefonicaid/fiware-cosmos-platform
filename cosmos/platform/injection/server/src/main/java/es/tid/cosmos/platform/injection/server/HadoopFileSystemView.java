package es.tid.cosmos.platform.injection.server;

import java.io.IOException;
import java.net.URI;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.sshd.server.FileSystemView;
import org.apache.sshd.server.SshFile;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * HadoopFileSystemView
 * see COPYRIGHT or LICENSE for terms of use
 *
 * @author logc
 * @since  04/05/12
 */
public class HadoopFileSystemView implements FileSystemView {
    private String homePath;
    private FileSystem hadoopFS;
    private final String userName;
    private final Logger LOG = LoggerFactory
            .getLogger(HadoopFileSystemView.class);

    public HadoopFileSystemView(String userName, Configuration configuration) {
        this.userName = userName;
        try {
            this.hadoopFS = FileSystem.get(
                    URI.create(configuration.get("fs.default.name")),
                    configuration, userName);
            this.homePath = this.hadoopFS.getHomeDirectory().toString()
                    .replaceFirst(this.hadoopFS.getUri().toString(), "");
        } catch (InterruptedException e) {
            LOG.error(e.getMessage(), e);
        } catch (IOException e) {
            LOG.error(e.getMessage(), e);
        }
    }

    @Override
    public HadoopSshFile getFile(String file) {
        LOG.info("view asked for file " + file);
        try {
            return this.getFile("", file);
        } catch (IOException e) {
            LOG.error(e.getMessage(), e);
            return this.getFile(Path.CUR_DIR);
        } catch (InterruptedException e) {
            LOG.error(e.getMessage(), e);
            return this.getFile(Path.CUR_DIR);
        }
    }

    @Override
    public HadoopSshFile getFile(SshFile baseDir, String file) {
        LOG.info("view asked for ssh path " + baseDir.toString() +
                " and file " + file);
        String baseDirPath = baseDir.getAbsolutePath();
        try {
            return this.getFile(baseDirPath, file);
        } catch (IOException e) {
            LOG.error(e.getMessage(), e);
            return this.getFile(Path.CUR_DIR);
        } catch (InterruptedException e) {
            LOG.error(e.getMessage(), e);
            return this.getFile(Path.CUR_DIR);
        }
    }

    private HadoopSshFile getFile(String baseDir, String file)
            throws IOException, InterruptedException {
        LOG.info("view asked for dir " + baseDir + " and file " + file);
        if (baseDir.isEmpty() && file.isEmpty()) {
            throw new IllegalArgumentException("filesystem view not possible" +
                    "for empty directory and filename");
        }
        if (baseDir.isEmpty() && file.equals(Path.CUR_DIR)) {
            baseDir = this.homePath;
            file = "";
            LOG.info("redirecting to homepath: " + this.homePath);
        }
        String wholePath = baseDir + file;
        if (!baseDir.endsWith(Path.SEPARATOR) &&
                !file.startsWith(Path.SEPARATOR)) {
            wholePath = baseDir + Path.SEPARATOR + file;
        }
        LOG.info("trying to get a view for path: " + wholePath);
        return new HadoopSshFile(wholePath, this.userName, this.hadoopFS);
    }
}
