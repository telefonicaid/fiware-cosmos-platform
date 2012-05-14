package es.tid.cosmos.platform.injection.server;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.URI;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileStatus;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.fs.permission.FsAction;
import org.apache.hadoop.fs.permission.FsPermission;
import org.apache.hadoop.security.AccessControlException;
import org.apache.sshd.server.SshFile;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 *
 * @author logc
 * @since  04/05/12
 */
public class HadoopSshFile implements SshFile {
    private String fileName;
    private String userName;
    private Path hadoopPath;
    private Configuration configuration;
    private final FileSystem hadoopFS;
    private final Logger LOG = LoggerFactory.getLogger(HadoopSshFile.class);
    private final String OS_SEP = System.getProperty("file.separator");

    protected HadoopSshFile(final String fileName, final String userName,
                            Configuration conf, FileSystem fs)
            throws IOException, InterruptedException {
        if (fileName.isEmpty()) {
            throw new IllegalArgumentException("fileName cannot be empty");
//        } else if (!fileName.startsWith(OS_SEP)) {
//            throw new IllegalArgumentException(String.format(
//                    "fileName must be an absolute path, but got %s", fileName));
        }
        this.fileName = fileName;
        this.userName = userName;
        this.configuration = conf;
        this.hadoopPath = new Path(fileName);
        this.hadoopFS = FileSystem.get(URI.create(conf.get("fs.default.name")),
                conf, userName);
    }

    @Override
    public String getAbsolutePath() {
        String fullName = this.fileName;
        if (fullName.length() != 1 &&
                (fullName.endsWith("/"))) {
            fullName = fullName.substring(0, fullName.length() - 1);
        }
        return fullName;
    }

    @Override
    public String getName() {
        return this.hadoopPath.getName();
    }

    @Override
    public boolean isDirectory() {
        try {
            return this.hadoopFS.getFileStatus(this.hadoopPath).isDir();
        } catch (IOException e) {
            LOG.error(e.getMessage(), e);
            return false;
        }
    }

    @Override
    public boolean isFile() {
        try {
            return this.hadoopFS.isFile(this.hadoopPath);
        } catch (IOException e) {
            LOG.error(e.getMessage(), e);
            return false;
        }
    }

    @Override
    public boolean doesExist() {
        try {
            return this.hadoopFS.exists(this.hadoopPath);
        } catch (IOException e) {
            LOG.error(e.getMessage(), e);
            return false;
        }
    }

    private boolean isAllowed(FsAction queriedFsAction) {
        LOG.info("looking for action " + queriedFsAction.toString() +
                " on path " + this.hadoopPath.toString());
        try {
            String pathOwner = this.hadoopFS.getFileStatus(this.hadoopPath)
                    .getOwner();
            boolean ans = false;
            if (pathOwner.equals(this.userName)) {
                ans = this.hadoopFS.getFileStatus(this.hadoopPath).getPermission()
                        .getUserAction().implies(queriedFsAction);
            } else {
                ans = this.hadoopFS.getFileStatus(this.hadoopPath).getPermission()
                        .getOtherAction().implies(queriedFsAction);
            }
            LOG.info("user action allowed?: " + ans);
            return ans;
        } catch (IOException e) {
            LOG.error(e.getMessage(), e);
            return false;
        }
    }

    @Override
    public boolean isReadable() {
        return isAllowed(FsAction.READ);
    }

    @Override
    public boolean isWritable() {
        if (this.looksLikeFilePath() && !this.doesExist()) {
            return (this.getParentFile().isWritable());
        }
        return isAllowed(FsAction.WRITE);
    }

    private boolean looksLikeFilePath() {
        return !this.getAbsolutePath().endsWith(OS_SEP);
    }

    @Override
    public boolean isExecutable() {
        return isAllowed(FsAction.EXECUTE);
    }

    @Override
    public boolean isRemovable() {
        return isAllowed(FsAction.ALL);
    }

    @Override
    public SshFile getParentFile() {
        try {
            return new HadoopSshFile(this.hadoopPath.getParent().toString(),
                    this.userName, this.configuration, this.hadoopFS);
        } catch (IOException e) {
            LOG.error(e.getMessage(), e);
            return null;
        } catch (InterruptedException e) {
            LOG.error(e.getMessage());
            return null;
        }
    }

    @Override
    public long getLastModified() {
        try {
            FileStatus fileStatus = this.hadoopFS.getFileStatus(this.hadoopPath);
            return fileStatus.getModificationTime();
        } catch (IOException e) {
            LOG.error(e.getMessage(), e);
            return 0;
        }
    }

    @Override
    public boolean setLastModified(long time) {
        try {
            this.hadoopFS.setTimes(this.hadoopPath, time, System.currentTimeMillis());
            return true;
        } catch (IOException e) {
            LOG.error(e.getMessage(), e);
        }
        return false;
    }

    @Override
    public long getSize() {
        try {
            if (this.doesExist()) {
                FileStatus fileStatus =
                        this.hadoopFS.getFileStatus(this.hadoopPath);
                return fileStatus.getLen();
            } else {
                return 0;
            }
        } catch (IOException e) {
            LOG.error(e.getMessage(), e);
        }
        return 0;
    }

    @Override
    public boolean mkdir() {
        if (!this.doesExist() && this.isDirectory()) {
            try {
                return this.hadoopFS.mkdirs(this.hadoopPath);
            } catch (IOException e) {
                LOG.error(e.getMessage(), e);
            }
        }
        return false;
    }

    @Override
    public boolean delete() {
        try {
            return this.hadoopFS.delete(this.hadoopPath, this.isDirectory());
        } catch (IOException e) {
            LOG.error(e.getMessage(), e);
            return false;
        }
    }

    @Override
    public boolean create() throws IOException {
        return this.hadoopFS.createNewFile(this.hadoopPath);
    }

    @Override
    public void truncate() throws IOException {
        boolean do_overwrite = true;
        this.hadoopFS.create(this.hadoopPath, do_overwrite);
    }

    @Override
    public boolean move(SshFile destination) {
        try {
            Path dest = new Path(destination.getAbsolutePath());
            return this.hadoopFS.rename(this.hadoopPath, dest);
        } catch (IOException e) {
            LOG.error(e.getMessage(), e);
        }
        return false;
    }

    @Override
    public List<SshFile> listSshFiles() {
        LOG.info(String.format("listing files at %s", this.getAbsolutePath()));
//        if (!this.isDirectory() || !this.isReadable()){
        if (!this.isReadable()){
            throw new InjectionUnathorizedPathException(String.format(
                    "User %s not authorized to list directory %s",
                    this.userName, this.getAbsolutePath()));
        }
        LOG.info("got here");
        try {
            FileStatus[] fileStatuses = this.hadoopFS.listStatus(this.hadoopPath);
            SshFile[] ans = new SshFile[fileStatuses.length];
            int i = 0;
            for (FileStatus fileStatus : fileStatuses) {
                String fileName = fileStatus.getPath().getName();
                String fileOwner = fileStatus.getOwner();
                ans[i] = new HadoopSshFile(joinPath(fileName), fileOwner,
                        this.configuration, this.hadoopFS);
                i++;
            }
            return Collections.unmodifiableList(Arrays.asList(ans));
        } catch (AccessControlException e) {
            LOG.error(e.getMessage(), e);
            try {
                SshFile[] ans = new SshFile[1];
                ans[0] = new HadoopSshFile(Path.CUR_DIR,
                        this.userName, this.configuration, this.hadoopFS);
                return Collections.unmodifiableList(Arrays.asList(ans));
            } catch (IOException e1) {
                LOG.error(e1.getMessage(), e);
            } catch (InterruptedException e1) {
                LOG.error(e1.getMessage(), e);
            }
        } catch (IOException e) {
            LOG.error(e.getMessage(), e);
            return null;
        } catch (InterruptedException e) {
            LOG.error(e.getMessage());
            return null;
        }
        return null;
    }

    private String joinPath(String fileName) {
        String absPath = this.getAbsolutePath();
        return ((absPath.endsWith(OS_SEP)) ? absPath
                                           : (absPath + OS_SEP)) + fileName;
    }

    @Override
    public OutputStream createOutputStream(long offset) throws IOException {
        // TODO: when offset != 0, append with bufferSize?
        return this.hadoopFS.create(this.hadoopPath);
    }

    @Override
    public InputStream createInputStream(long offset) throws IOException {
        return this.hadoopFS.open(this.hadoopPath);
    }

    @Override
    public void handleClose() throws IOException {
        // Noop
    }
}