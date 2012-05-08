package es.tid.cosmos.platform.injection.server;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileStatus;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.fs.permission.FsAction;
import org.apache.hadoop.fs.permission.FsPermission;
import org.apache.sshd.server.SshFile;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 *
 * @author logc
 */
public class HadoopSshFile implements SshFile {
    private String fileName;
    private String userName;
    private int userId;
    private Path hadoopPath;
    private FileSystem hadoopFS;
    private final Logger LOG;
    private final String OS_SEP = System.getProperty("file.separator");

    protected HadoopSshFile(final String fileName, final String userName,
                            final int userId) throws IOException {

        if (fileName.isEmpty()) {
            throw new IllegalArgumentException("fileName cannot be empty");
        } else if (!fileName.startsWith(OS_SEP)) {
            throw new IllegalArgumentException(String.format(
                    "fileName must be an absolute path, but got %s",
                    fileName));
        }
        this.fileName = fileName;
        this.userName = userName;
        this.userId = userId;
        this.hadoopPath = new Path(fileName);
        Configuration conf = new Configuration();
        this.hadoopFS = FileSystem.get(conf);
        LOG = LoggerFactory.getLogger(HadoopSshFile.class);
    }

    @Override
    public String getAbsolutePath() {
        String fullName = this.fileName;
        if (fullName.length() != 1 &&
                (fullName.charAt(fullName.length() - 1) == '/')) {
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
        FileStatus fileStatus;
        try {
            fileStatus = this.hadoopFS.getFileStatus(
                    this.hadoopPath);
            return fileStatus.isDir();
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
            FileStatus fileStatus = this.hadoopFS.getFileStatus(this.hadoopPath);
            FsPermission fsPermission = fileStatus.getPermission();
            FsAction actualFsAction = fsPermission.getUserAction();
            return actualFsAction.implies(queriedFsAction);
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
                    this.userName, this.userId);
        } catch (IOException e) {
            LOG.error(e.getMessage(), e);
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
        boolean recursively = false;
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
        if (!this.isDirectory()){
            return null;
        }
        try {
            FileStatus[] fileStatuses = this.hadoopFS.listStatus(this.hadoopPath);
            SshFile[] ans = new SshFile[fileStatuses.length];
            int i = 0;
            for (FileStatus fileStatus : fileStatuses) {
                String fileName = fileStatus.getPath().getName();
                String fileOwner = fileStatus.getOwner();
                // TODO: correct when access to user database is cleared
                int fileOwnerId = this.userId;
                ans[i] = new HadoopSshFile(
                        joinPath(fileName), fileOwner,
                        fileOwnerId);
                i++;
            }
            return Collections.unmodifiableList(Arrays.asList(ans));
        } catch (IOException e) {
            LOG.error(e.getMessage(), e);
            return null;
        }
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