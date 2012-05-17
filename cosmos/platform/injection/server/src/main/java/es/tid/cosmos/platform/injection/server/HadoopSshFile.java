package es.tid.cosmos.platform.injection.server;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

import org.apache.hadoop.fs.FSDataOutputStream;
import org.apache.hadoop.fs.FileStatus;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.fs.permission.FsAction;
import org.apache.hadoop.security.AccessControlException;
import org.apache.sshd.server.SshFile;

import es.tid.cosmos.base.util.Logger;

/**
 * HadoopSshFile
 *
 * @author logc
 * @since  CTP 2
 */
public class HadoopSshFile implements SshFile {
    private String userName;
    private Path hadoopPath;
    private FSDataOutputStream fsDataOutputStream;
    private final FileSystem hadoopFS;
    private final org.apache.log4j.Logger LOG = Logger.get(HadoopSshFile.class);

    protected HadoopSshFile(final String fileName, final String userName,
                            FileSystem hadoopFS)
            throws IOException, InterruptedException {
        this.hadoopPath = new Path(fileName);
        this.userName = userName;
        this.hadoopFS = hadoopFS;
    }

    /**
     * Get the whole tree of directories from root until the current path
     *
     * @return a string of directories until the current path
     */
    @Override
    public String getAbsolutePath() {
        LOG.trace("getting absolute path");
        return this.hadoopPath.toString();
    }

    /**
     * Get the last part of the path, i.e. the file name if the path is a file
     * path
     *
     * @return the file name
     */
    @Override
    public String getName() {
        LOG.trace("getting name");
        return this.hadoopPath.getName();
    }

    /**
     * Answer if the current path is a directory path
     *
     * @return is this a directory path?
     */
    @Override
    public boolean isDirectory() {
        LOG.trace("is this a directory?");
        try {
            return this.hadoopFS.getFileStatus(this.hadoopPath).isDir();
        } catch (IOException e) {
            LOG.error(e.getMessage(), e);
            return false;
        }
    }

    /**
     * Answer if the current path is a file path
     *
     * @return is this a file path?
     */
    @Override
    public boolean isFile() {
        LOG.trace("is this a file?");
        try {
            return this.hadoopFS.isFile(this.hadoopPath);
        } catch (IOException e) {
            LOG.error(e.getMessage(), e);
            return false;
        }
    }

    /**
     * Answer whether the current path exists or is just a representation for
     * something to be later created.
     *
     * @return does this path exist?
     */
    @Override
    public boolean doesExist() {
        LOG.trace("does this exist?");
        try {
            return this.hadoopFS.exists(this.hadoopPath);
        } catch (IOException e) {
            LOG.error(e.getMessage(), e);
            return false;
        }
    }

    /**
     * Answer if the current user permissions on the current path are enough to
     * perform a requested action. This method is the common basis for the
     * following permission-related methods, such as isReadable, isWritable ...
     *
     * @param queriedFsAction an action such as read, write ...
     * @return                is this user allowed to perform this action?
     */
    private boolean isAllowed(FsAction queriedFsAction) {
        LOG.trace("is this allowed?");
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
            LOG.trace(ans? "yes" : "no");
            return ans;
        } catch (IOException e) {
            LOG.error(e.getMessage(), e);
            return false;
        }
    }

    /**
     * Answer if this path is readable by the current user.
     *
     * @return is this user allowed to read this path?
     */
    @Override
    public boolean isReadable() {
        LOG.trace("is this readable?");
        return isAllowed(FsAction.READ);
    }

    /**
     * Answer if this path is writable by the current user. If the current path
     * does not exist, look for the same permission in the parent path.
     *
     * @return is this user allowed to write to this path?
     */
    @Override
    public boolean isWritable() {
        LOG.trace("is this writable?");
        if (this.doesExist()){
            return isAllowed(FsAction.WRITE);
        } else {
            return this.getParentFile().isWritable();
        }
    }

    /**
     * Answer if the path is executable by the current user. HDFS does not have
     * any executable files, but this is still delegated to its API in case
     * there are executable files in the future.
     *
     * References
     * In contrast to the POSIX model, there are no sticky, setuid or setgid
     * bits for files as there is no notion of executable files.
     * http://hadoop.apache.org/common/docs/r0.20.2/hdfs_permissions_guide.html
     *
     * @return is this user allowed to execute this path?
     */
    @Override
    public boolean isExecutable() {
        LOG.trace("is this executable?");
        return isAllowed(FsAction.EXECUTE);
    }

    /**
     * Answer if this path is removable by the current user
     *
     * References
     *
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
        LOG.trace("is this removable?");
        return isAllowed(FsAction.WRITE);
    }

    /**
     * Answer which path is the parent of the current path in the filesystem
     * tree
     *
     * @return the parent path of the current path
     */
    @Override
    public SshFile getParentFile() {
        LOG.trace("getting parent file");
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

    /**
     * Answer when was a file modified for the last time
     *
     * @return the last modification time in milliseconds
     */
    @Override
    public long getLastModified() {
        LOG.trace("getting last modified");
        try {
            FileStatus fileStatus = this.hadoopFS.getFileStatus(this.hadoopPath);
            return fileStatus.getModificationTime();
        } catch (IOException e) {
            LOG.error(e.getMessage(), e);
            return 0;
        }
    }

    /**
     * Change the modification time of a path
     *
     * @param  time new modification time in milliseconds
     * @return was the modification time successfully changed?
     */
    @Override
    public boolean setLastModified(long time) {
        LOG.trace("setting last modified");
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

    /**
     * Answer how long is a file in bytes
     *
     * @return file length in bytes
     */
    @Override
    public long getSize() {
        LOG.trace("getting size");
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

    /**
     * Create a new directory
     *
     * @return was the directory successfully created?
     */
    @Override
    public boolean mkdir() {
        LOG.trace("trying to make the dir: " + this.getAbsolutePath());
        try {
            if (this.getParentFile().doesExist() &&
                    this.getParentFile().isWritable()) {
                if (this.hadoopFS.mkdirs(this.hadoopPath)) {
                    return true;
                }
            }
            return false;
        } catch (IOException e) {
            LOG.error(e.getMessage(), e);
            return false;
        }
    }

    /**
     * Delete a file
     *
     * @return was the file successfully deleted?
     */
    @Override
    public boolean delete() {
        LOG.trace("deleting");
        try {
            return this.hadoopFS.delete(this.hadoopPath, this.isDirectory());
        } catch (IOException e) {
            LOG.error(e.getMessage(), e);
            return false;
        }
    }

    /**
     * Create a new file
     *
     * @return was the file successfully created?
     * @throws IOException
     */
    @Override
    public boolean create() throws IOException {
        LOG.trace("creating");
        return this.hadoopFS.createNewFile(this.hadoopPath);
    }

    /**
     * Discard all contents in a path, setting its size to 0 but keeping it as
     * existing.
     *
     * @throws IOException
     */
    @Override
    public void truncate() throws IOException {
        LOG.trace("truncating");
        FSDataOutputStream tmp = this.hadoopFS.create(this.hadoopPath, true);
        tmp.close();
    }

    /**
     * Move this path to the destination specified in this method's signature,
     * if permissions of this user on destination are sufficient.
     *
     * @param destination a path where this file is requested to be moved
     * @return            was the path moved?
     */
    @Override
    public boolean move(SshFile destination) {
        LOG.trace("moving");
        try {
            Path dest = new Path(destination.getAbsolutePath());
            return this.hadoopFS.rename(this.hadoopPath, dest);
        } catch (IOException e) {
            LOG.error(e.getMessage(), e);
        }
        return false;
    }

    /**
     * List files found within a directory
     *
     * @return a List of SshFiles
     */
    @Override
    public List<SshFile> listSshFiles() {
        LOG.trace(String.format("listing files at %s", this.getAbsolutePath()));
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
        LOG.trace("joining to this path");
        String absPath = this.getAbsolutePath();
        return ((absPath.endsWith(
                 Path.SEPARATOR)) ? absPath
                                  : (absPath + Path.SEPARATOR)) + fileName;
    }

    /**
     * Create output stream for writing
     *
     * @param  offset  a number of bytes to be skipped
     * @return         a stream to write bytes to
     * @throws IOException
     */
    @Override
    public OutputStream createOutputStream(long offset) throws IOException {
        LOG.trace("trying to write to path:" + this.hadoopPath.toString());
        if (!this.isWritable()) {
            throw new IOException("No write permission for: " +
                    this.getAbsolutePath());
        }
        // TODO: when offset != 0, append with bufferSize?
        this.fsDataOutputStream = this.hadoopFS.create(this.hadoopPath);
        return this.fsDataOutputStream;
    }

    /**
     * Create input stream for reading
     *
     * @param  offset a number of bytes to be skipped
     * @return        a stream to read bytes from
     * @throws IOException
     */
    @Override
    public InputStream createInputStream(long offset) throws IOException {
        LOG.trace("trying to read from path: " + this.hadoopPath.toString());
        if (!this.isReadable()) {
            throw new IOException("No read permission for: " +
                    this.getAbsolutePath());
        }
        // TODO: when offset != 0, append with bufferSize?
        return this.hadoopFS.open(this.hadoopPath);
    }

    /**
     * Close a file handle after reading from or writing to it
     *
     * @throws IOException
     */
    @Override
    public void handleClose() throws IOException {
        LOG.trace("trying to close the handle for path: " +
                this.hadoopPath.toString());
        try {
            this.fsDataOutputStream.close();
        } catch (Exception e) {
            LOG.info("closed path that was not open", e);
        }
    }
}