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

/**
 *
 * @author logc
 */
public class HadoopSshFile implements SshFile {
    // TODO: which variables can be declared final?
    private String osSep = System.getProperty("file.separator");
    private String fileName;
    private String userName;
    private int userId;
    private Path hadoopPath;
    private FileSystem hadoopFS;

    protected HadoopSshFile(final String fileName, final String userName,
                            final int userId) throws IOException {
        if (fileName.length() == 0) {
            throw new IllegalArgumentException("fileName cannot be empty");
        } else if (!Character.toString(fileName.charAt(0)).equals(this.osSep)) {
            throw new IllegalArgumentException(
                    "fileName must be an absolute path");
        }

        this.fileName = fileName;
        this.userName = userName;
        this.userId = userId;
        this.hadoopPath = new Path(fileName);
        Configuration conf = new Configuration();
        this.hadoopFS = FileSystem.get(conf);

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
        FileStatus fileStatus = null;
        try {
            fileStatus = this.hadoopFS.getFileStatus(
                    this.hadoopPath);
            return fileStatus.isDir();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return false;
    }

    @Override
    public boolean isFile() {
        try {
            return this.hadoopFS.isFile(this.hadoopPath);
        } catch (IOException e) {
            e.printStackTrace();
        }
        return false;
    }

    @Override
    public boolean doesExist() {
        try {
            return this.hadoopFS.exists(this.hadoopPath);
        } catch (IOException e) {
            e.printStackTrace();
        }
        return false;
    }

    @Override
    public boolean isReadable() {
        try {
            FileStatus fileStatus = this.hadoopFS.getFileStatus(this.hadoopPath);
            FsPermission fsPermission = fileStatus.getPermission();
            FsAction fsAction = fsPermission.getUserAction();
            return fsAction.implies(FsAction.READ);
        } catch (IOException e) {
            e.printStackTrace();
        }
        return false;
    }

    @Override
    public boolean isWritable() {
        try {
            FileStatus fileStatus = this.hadoopFS.getFileStatus(this.hadoopPath);
            FsPermission fsPermission = fileStatus.getPermission();
            FsAction fsAction = fsPermission.getUserAction();
            return fsAction.implies(FsAction.WRITE);
        } catch (IOException e) {
            e.printStackTrace();
        }
        return false;
    }

    @Override
    public boolean isExecutable() {
        try {
            FileStatus fileStatus = this.hadoopFS.getFileStatus(this.hadoopPath);
            FsPermission fsPermission = fileStatus.getPermission();
            FsAction fsAction = fsPermission.getUserAction();
            return fsAction.implies(FsAction.EXECUTE);
        } catch (IOException e) {
            e.printStackTrace();
        }
        return false;
    }

    @Override
    public boolean isRemovable() {
        try {
            FileStatus fileStatus = this.hadoopFS.getFileStatus(this.hadoopPath);
            FsPermission fsPermission = fileStatus.getPermission();
            FsAction fsAction = fsPermission.getUserAction();
            return fsAction.implies(FsAction.ALL);
        } catch (IOException e) {
            e.printStackTrace();
        }
        return false;
    }

    @Override
    public SshFile getParentFile() {
        try {
            return new HadoopSshFile(this.hadoopPath.getParent().getName(), this.userName, this.userId);
        } catch (IOException e) {
            e.printStackTrace();
        }
        return null;
    }

    @Override
    public long getLastModified() {
        try {
            FileStatus fileStatus = this.hadoopFS.getFileStatus(this.hadoopPath);
            return fileStatus.getModificationTime();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return 0;
    }

    @Override
    public boolean setLastModified(long time) {
        try {
            this.hadoopFS.setTimes(this.hadoopPath, time, System.currentTimeMillis());
            return true;
        } catch (IOException e) {
            e.printStackTrace();
        }
        return false;
    }

    @Override
    public long getSize() {
        try {
            if (this.doesExist()) {
                FileStatus fileStatus = this.hadoopFS.getFileStatus(this.hadoopPath);
                return fileStatus.getLen();
            } else {
                return 0;
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        return 0;
    }

    @Override
    public boolean mkdir() {
        if (!this.doesExist() && this.isDirectory()) {
            try {
                return this.hadoopFS.mkdirs(this.hadoopPath);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        return false;
    }

    @Override
    public boolean delete() {
        boolean recursively = false;
        try {
            if (this.isDirectory()) {
                recursively = true;
            }
            return this.hadoopFS.delete(this.hadoopPath, recursively);
        } catch (IOException e) {
            e.printStackTrace();
        }
        return false;
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
            e.printStackTrace();
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
            HadoopSshFile[] ans = new HadoopSshFile[fileStatuses.length];
            int i = 0;
            for (FileStatus fileStatus : fileStatuses) {
                String fileName = fileStatus.getPath().getName();
                String fileOwner = fileStatus.getOwner();
                // TODO: correct when access to user database is cleared
                int fileOwnerId = this.userId;
                HadoopSshFile tmp = new HadoopSshFile(fileName, fileOwner,
                        fileOwnerId);
                ans[i] = tmp;
                i++;
            }
            // TODO: check performance; taken from native filesystem + cast
            return (List<SshFile>) Collections.unmodifiableList(Arrays.asList(ans));
        } catch (IOException e) {
            e.printStackTrace();
        }
        return null;
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
        // TODO: implement handleClose
    }
}
