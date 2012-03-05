package es.tid.bdp.sftp.server.filesystem.hadoop;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

import org.apache.hadoop.fs.FileStatus;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.sshd.server.SshFile;
import org.apache.sshd.server.filesystem.NativeSshFile;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.hadoop.compression.lzo.LzopCodec;

import es.tid.bdp.utils.FileSystemControllerAbstract;
import es.tid.bdp.utils.data.BdpFileDescriptor;
import es.tid.bdp.utils.io.output.ProtoBufOutStream;
import es.tid.bdp.utils.parse.ParserAbstract;

/**
 * 
 * @author rgc
 *
 */
public class HdfsSshFile implements SshFile {

    private final Logger LOG = LoggerFactory.getLogger(NativeSshFile.class);

    // the file name with respect to the user root.
    // The path separator character will be '/' and
    // it will always begin with '/'.
    private String fileName;
    private String userName;

    private Path path;
    private FileSystemControllerAbstract hfdsCtrl;

    private FileSystem hdfs;

    private OutputStream outputStream;

    private BdpFileDescriptor descriptor;

    public HdfsSshFile(final FileSystem hdfs,
            final FileSystemControllerAbstract hfdsCtrl, final String userName,
            final String fileName) {
        if (hdfs == null) {
            LOG.error("Hdfs can not be null");
            throw new IllegalArgumentException("Hdfs can not be null");
        }
        if (fileName == null) {
            LOG.error("fileName can not be null");
            throw new IllegalArgumentException("fileName can not be null");
        }
        if (fileName.length() == 0) {
            LOG.error("fileName can not be empty");
            throw new IllegalArgumentException("fileName can not be empty");
        } else if (fileName.charAt(0) != '/') {
            LOG.error("fileName must be an absolute path");
            throw new IllegalArgumentException(
                    "fileName must be an absolute path");
        }

        this.descriptor = hfdsCtrl.build(userName, fileName);
        this.fileName = fileName;
        this.path = new Path(fileName);
        this.hdfs = hdfs;
        this.hfdsCtrl = hfdsCtrl;
    }

    /*
     * (non-Javadoc)
     * 
     * @see org.apache.sshd.server.SshFile#getAbsolutePath()
     */
    @Override
    public String getAbsolutePath() {
        // strip the last '/' if necessary
        String fullName = fileName;
        int filelen = fullName.length();
        if ((filelen != 1) && (fullName.charAt(filelen - 1) == '/')) {
            fullName = fullName.substring(0, filelen - 1);
        }
        return fullName;
    }

    /*
     * (non-Javadoc)
     * 
     * @see org.apache.sshd.server.SshFile#getName()
     */
    @Override
    public String getName() {
        // root - the short name will be '/'
        if (fileName.equals("/")) {
            return "/";
        }

        // strip the last '/'
        String shortName = fileName;
        int filelen = fileName.length();
        if (shortName.charAt(filelen - 1) == '/') {
            shortName = shortName.substring(0, filelen - 1);
        }

        // return from the last '/'
        int slashIndex = shortName.lastIndexOf('/');
        if (slashIndex != -1) {
            shortName = shortName.substring(slashIndex + 1);
        }
        return shortName;
    }

    /*
     * (non-Javadoc)
     * 
     * @see org.apache.sshd.server.SshFile#isDirectory()
     */
    @Override
    public boolean isDirectory() {
        try {
            return this.hdfs.getFileStatus(path).isDir();
        } catch (IOException e) {
            LOG.error("Error int he access to HDFS");
            throw new RuntimeException(e);
        }
    }

    /*
     * (non-Javadoc)
     * 
     * @see org.apache.sshd.server.SshFile#isFile()
     */
    @Override
    public boolean isFile() {
        try {
            return !this.hdfs.getFileStatus(path).isDir();
        } catch (IOException e) {
            LOG.error("Error int he access to HDFS");
            throw new RuntimeException(e);
        }
    }

    /*
     * (non-Javadoc)
     * 
     * @see org.apache.sshd.server.SshFile#doesExist()
     */
    @Override
    public boolean doesExist() {
        try {
            return hdfs.exists(path);
        } catch (IOException e) {
            LOG.error("Error int he access to HDFS");
            throw new RuntimeException(e);
        }
    }

    /*
     * (non-Javadoc)
     * 
     * @see org.apache.sshd.server.SshFile#isReadable()
     */
    @Override
    public boolean isReadable() {
        return descriptor.isReadable();
    }

    /*
     * (non-Javadoc)
     * 
     * @see org.apache.sshd.server.SshFile#isWritable()
     */
    @Override
    public boolean isWritable() {
        return descriptor.isWritable();
    }

    /*
     * (non-Javadoc)
     * 
     * @see org.apache.sshd.server.SshFile#isExecutable()
     */
    @Override
    public boolean isExecutable() {
        // TODO rgc: Not implemented
        throw new RuntimeException("isExecutable is not implemented");
    }

    /*
     * (non-Javadoc)
     * 
     * @see org.apache.sshd.server.SshFile#isRemovable()
     */
    @Override
    public boolean isRemovable() {
        // TODO rgc: Not implemented
        throw new RuntimeException("isExecutable is not implemented");
    }

    /*
     * (non-Javadoc)
     * 
     * @see org.apache.sshd.server.SshFile#getParentFile()
     */
    @Override
    public SshFile getParentFile() {
        int indexOfSlash = getAbsolutePath().lastIndexOf('/');
        String parentFullName;
        if (indexOfSlash == 0) {
            parentFullName = "/";
        } else {
            parentFullName = getAbsolutePath().substring(0, indexOfSlash);
        }
        return new HdfsSshFile(hdfs, hfdsCtrl, userName, parentFullName);
    }

    /*
     * (non-Javadoc)
     * 
     * @see org.apache.sshd.server.SshFile#getLastModified()
     */
    @Override
    public long getLastModified() {
        try {
            FileStatus fileStatus = hdfs.getFileStatus(path);
            return fileStatus.getModificationTime();
        } catch (Exception e) {
            LOG.error("Error int he access to HDFS");
            throw new RuntimeException(e);
        }
    }

    /*
     * (non-Javadoc)
     * 
     * @see org.apache.sshd.server.SshFile#setLastModified(long)
     */
    @Override
    public boolean setLastModified(long time) {
        try {
            hdfs.setTimes(path, time, time);
            return true;
        } catch (Exception e) {
            return false;
        }
    }

    /*
     * (non-Javadoc)
     * 
     * @see org.apache.sshd.server.SshFile#getSize()
     */
    @Override
    public long getSize() {
        try {
            FileStatus fileStatus = hdfs.getFileStatus(path);
            return fileStatus.getLen();
        } catch (Exception e) {
            LOG.error("Error int he access to HDFS");
            throw new RuntimeException(e);
        }
    }

    /*
     * (non-Javadoc)
     * 
     * @see org.apache.sshd.server.SshFile#mkdir()
     */
    @Override
    public boolean mkdir() {
        boolean retVal = false;
        if (isWritable()) {
            try {
                retVal = hdfs.mkdirs(path);
            } catch (IOException e) {
                LOG.error("Error int he access to HDFS");
                throw new RuntimeException(e);
            }
        }
        return retVal;
    }

    /*
     * (non-Javadoc)
     * 
     * @see org.apache.sshd.server.SshFile#delete()
     */
    @Override
    public boolean delete() {
        try {
            return hdfs.delete(path, true);
        } catch (Exception e) {
            return false;
        }
    }

    /*
     * (non-Javadoc)
     * @see org.apache.sshd.server.SshFile#create()
     */
    @Override
    public boolean create() throws IOException {
        outputStream = hdfs.create(path);
        return true;
    }

    /*
     * (non-Javadoc)
     * @see org.apache.sshd.server.SshFile#truncate()
     */
    @Override
    public void truncate() throws IOException {
        // TODO rgc: Not implemented
        throw new RuntimeException("truncate is not implemented");
    }

    /*
     * (non-Javadoc)
     * @see org.apache.sshd.server.SshFile#move(org.apache.sshd.server.SshFile)
     */
    @Override
    public boolean move(SshFile destination) {
        // TODO rgc: Not implemented
        throw new RuntimeException("move is not implemented");
    }

    /*
     * (non-Javadoc)
     * @see org.apache.sshd.server.SshFile#listSshFiles()
     */
    @Override
    public List<SshFile> listSshFiles() {
        // is a directory
        if (!isDirectory()) {
            return Collections.unmodifiableList(new ArrayList<SshFile>());
        }

        // directory - return all the files
        FileStatus[] elements;
        try {
            elements = hdfs.listStatus(path);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
        if (elements == null) {
            return Collections.unmodifiableList(new ArrayList<SshFile>());
        }

        // make sure the files are returned in order
        Arrays.sort(elements, new Comparator<FileStatus>() {
            public int compare(FileStatus f1, FileStatus f2) {
                return f1.getPath().getName().compareTo(f2.getPath().getName());
            }
        });

        // get the virtual name of the base directory
        String virtualFileStr = getAbsolutePath();
        if (virtualFileStr.charAt(virtualFileStr.length() - 1) != '/') {
            virtualFileStr += '/';
        }

        // now return all the files under the directory
        SshFile[] virtualFiles = new SshFile[elements.length];
        for (int i = 0; i < elements.length; ++i) {
            Path fileObj = elements[i].getPath();
            String fileName = virtualFileStr + fileObj.getName();
            virtualFiles[i] = new HdfsSshFile(hdfs, hfdsCtrl, userName,
                    fileName);
        }
        return Collections.unmodifiableList(Arrays.asList(virtualFiles));
    }

    /*
     * (non-Javadoc)
     * @see org.apache.sshd.server.SshFile#createOutputStream(long)
     */
    @Override
    public OutputStream createOutputStream(long offset) throws IOException {
        if (offset > 0) {
            throw new RuntimeException();
        }
        if (this.outputStream == null) {
            create();
        }

        if (this.descriptor.isCompressible()) {
            LzopCodec codec = new LzopCodec();
            codec.setConf(hdfs.getConf());
            outputStream = codec.createOutputStream(outputStream);
        }

        ParserAbstract parser = descriptor.getParser();
        if (parser != null) {
            outputStream = new ProtoBufOutStream(outputStream, parser);
        }
        return outputStream;
    }

    /*
     * (non-Javadoc)
     * @see org.apache.sshd.server.SshFile#createInputStream(long)
     */
    @Override
    public InputStream createInputStream(long offset) throws IOException {
        if (offset > 0) {
            throw new RuntimeException();
        }
        return hdfs.open(path);
    }

    /*
     * (non-Javadoc)
     * @see org.apache.sshd.server.SshFile#handleClose()
     */
    @Override
    public void handleClose() throws IOException {
        throw new RuntimeException("handleClose is not implemented");
    }
}
