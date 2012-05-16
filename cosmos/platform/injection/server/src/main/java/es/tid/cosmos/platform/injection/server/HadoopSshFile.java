package es.tid.cosmos.platform.injection.server;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

import org.apache.hadoop.fs.FileStatus;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.fs.permission.FsAction;
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
    private String userName;
    private Path hadoopPath;
    private final FileSystem hadoopFS;
    private final Logger LOG = LoggerFactory.getLogger(HadoopSshFile.class);

    protected HadoopSshFile(final String fileName, final String userName,
                            FileSystem hadoopFS)
            throws IOException, InterruptedException {
        this.hadoopPath = new Path(fileName);
        this.userName = userName;
        this.hadoopFS = hadoopFS;
    }

    @Override
    public String getAbsolutePath() {
        return this.hadoopPath.toString();
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
        try {
            String pathOwner = this.hadoopFS.getFileStatus(this.hadoopPath)
                    .getOwner();
            boolean ans;
            if (pathOwner.equals(this.userName)) {
                ans = this.hadoopFS.getFileStatus(this.hadoopPath).getPermission()
                        .getUserAction().implies(queriedFsAction);
            } else {
                ans = this.hadoopFS.getFileStatus(this.hadoopPath).getPermission()
                        .getOtherAction().implies(queriedFsAction);
            }
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
        return !this.getAbsolutePath().endsWith(Path.SEPARATOR);
    }

    @Override
    public boolean isExecutable() {
        return isAllowed(FsAction.EXECUTE);
    }

    /**
     * For directories, the r permission is required to list the contents of
     * the directory, the w permission is required to create or delete files or
     * directories, and the x permission is required to access a child of the
     * directory.
     * cf. http://hadoop.apache.org/common/docs/r0.20.0/hdfs_permissions_guide.html
     *
     * @return is this user allowed to remove this path?
     */
    @Override
    public boolean isRemovable() {
        return isAllowed(FsAction.WRITE);
    }

    @Override
    public SshFile getParentFile() {
        try {
            return new HadoopSshFile(this.hadoopPath.getParent().toString(),
                    this.userName, this.hadoopFS);
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
            /* Filesystem.setTimes(path, modification time, access time)
             *
             * Here it is supposed that setting the modification time is an
             * access to the file.
             */
            this.hadoopFS.setTimes(this.hadoopPath, time,
                    System.currentTimeMillis());
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
        try {
            return this.hadoopFS.mkdirs(this.hadoopPath);
        } catch (IOException e) {
            LOG.error(e.getMessage(), e);
            return false;
        }
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
        try {
            FileStatus[] fileStatuses = this.hadoopFS.listStatus(this.hadoopPath);
            SshFile[] ans = new SshFile[fileStatuses.length];
            int i = 0;
            for (FileStatus fileStatus : fileStatuses) {
                String fileName = fileStatus.getPath().getName();
                ans[i] = new HadoopSshFile(this.joinToThisPath(fileName),
                        this.userName, this.hadoopFS);
                i++;
            }
            return Collections.unmodifiableList(Arrays.asList(ans));
        } catch (AccessControlException e) {
            LOG.error(e.getMessage(), e);
            try {
                SshFile[] ans = new SshFile[1];
                ans[0] = new HadoopSshFile(Path.CUR_DIR,
                        this.userName, this.hadoopFS);
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

    private String joinToThisPath(String fileName) {
        String absPath = this.getAbsolutePath();
        return ((absPath.endsWith(
                 Path.SEPARATOR)) ? absPath
                                  : (absPath + Path.SEPARATOR)) + fileName;
    }

    @Override
    public OutputStream createOutputStream(long offset) throws IOException {
        // TODO: when offset != 0, append with bufferSize?
        LOG.info("trying to write to path:" + this.hadoopPath.toString());
        return this.hadoopFS.create(this.hadoopPath);
    }

    @Override
    public InputStream createInputStream(long offset) throws IOException {
        // TODO: when offset != 0, append with bufferSize?
        LOG.info("trying to read from path: " + this.hadoopPath.toString());
        return this.hadoopFS.open(this.hadoopPath);
    }

    @Override
    public void handleClose() throws IOException {
        LOG.info("trying to close the handle for path: " +
                this.hadoopPath.toString());
        // Noop
        LOG.info("but this is a noop");
    }
}