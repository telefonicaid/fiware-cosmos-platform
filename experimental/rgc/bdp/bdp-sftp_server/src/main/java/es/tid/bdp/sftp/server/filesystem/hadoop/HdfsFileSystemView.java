package es.tid.bdp.sftp.server.filesystem.hadoop;

import org.apache.hadoop.fs.FileSystem;
import org.apache.sshd.server.FileSystemView;
import org.apache.sshd.server.SshFile;
import org.apache.sshd.server.filesystem.NativeFileSystemFactory;
import org.apache.sshd.server.filesystem.NativeFileSystemView;
import org.apache.sshd.server.filesystem.NativeSshFile;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import es.tid.bdp.utils.FileSystemControllerAbstract;

/**
 * File system view based on hdfs file system. Here the root directory will be
 * user virtual root (/). It needs a controller for checking the access of the
 * users to the different files and directories
 * 
 * @author rgc
 * 
 */
public class HdfsFileSystemView implements FileSystemView {

    private final Logger LOG = LoggerFactory
            .getLogger(NativeFileSystemView.class);

    private String userName;
    private FileSystem hdfs;

    private String currDir;

    private FileSystemControllerAbstract hfdsCtrl;

    private boolean caseInsensitive = false;

    /**
     * Constructor - internal do not use directly, use
     * {@link NativeFileSystemFactory} instead
     */
    public HdfsFileSystemView(String userName, boolean caseInsensitive,
            FileSystem hdfs, FileSystemControllerAbstract hfdsCtrl) {
        if (userName == null) {
            throw new IllegalArgumentException("user can not be null");
        }

        this.caseInsensitive = caseInsensitive;

        this.currDir = "/";
        this.userName = userName;
        this.hdfs = hdfs;
        this.hfdsCtrl = hfdsCtrl;

        LOG.debug(
                "Hdfs filesystem view created for user \"{}\" with root \"{}\"",
                userName, currDir);
    }

    /**
     * Get file object.
     */
    public SshFile getFile(String file) {
        return getFile(currDir, file);
    }

    public SshFile getFile(SshFile baseDir, String file) {
        return getFile(baseDir.getAbsolutePath(), file);
    }

    protected SshFile getFile(String dir, String file) {
        // get actual file object
        String physicalName = NativeSshFile.getPhysicalName("/", dir, file,
                caseInsensitive);

        return new HdfsSshFile(hdfs, hfdsCtrl, userName, physicalName);
    }
}
