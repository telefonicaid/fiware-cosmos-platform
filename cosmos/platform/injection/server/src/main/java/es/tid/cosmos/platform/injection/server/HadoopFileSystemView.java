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
 *
 * @author logc
 */
public class HadoopFileSystemView implements FileSystemView {
    private Configuration configuration;
    private FileSystem hadoopFS;
    private String homePath;
    private final String userName;
    private final Logger LOG =
            LoggerFactory.getLogger(HadoopFileSystemView.class);

    public HadoopFileSystemView(String userName, Configuration configuration) {
        this.userName = userName;
        this.configuration = configuration;
        try {
            this.hadoopFS = FileSystem.get(
                    URI.create(configuration.get("fs.default.name")), configuration,
                    userName);
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
        if (baseDir.equals(Path.CUR_DIR) && file.equals(Path.CUR_DIR)) {
            baseDir = this.homePath;
            file = "";
            LOG.info(String.format("reformatting requested dir to %s, " +
                    "file to %s", baseDir, file));
        }
        try {
            LOG.info("trying to open the path: " + baseDir + file);
            HadoopSshFile ans = new HadoopSshFile(baseDir + file,
                    this.userName, this.configuration);
            if (!ans.isReadable()) {
                throw new InjectionUnathorizedPathException("user " +
                        this.userName + " not authorized to view " + file);

            } else {
                return ans;
            }
        } catch (InjectionUnathorizedPathException e) {
            LOG.error(e.getMessage(), e);
            return new HadoopSshFile(this.homePath, this.userName,
                    this.configuration);
        }
    }
}
