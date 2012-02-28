package es.tid.bdp.sftp.server.filesystem.hadoop;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.lang.reflect.Constructor;
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

import es.tid.bdp.utils.BuilderDdpFileDescriptorAbstract;
import es.tid.bdp.utils.data.BdpFileDescriptor;
import es.tid.bdp.utils.data.BdpFileDescriptor.BdpCompresion;
import es.tid.bdp.utils.io.output.ProtoBufOutStream;
import es.tid.bdp.utils.parse.ParserAbstract;

public class HdfsSshFile implements SshFile {

    private final Logger LOG = LoggerFactory.getLogger(NativeSshFile.class);

    // the file name with respect to the user root.
    // The path separator character will be '/' and
    // it will always begin with '/'.
    private String fileName;
    private String userName;

    private Path path;
    private BuilderDdpFileDescriptorAbstract builder;

    private FileSystem hdfs;

    private OutputStream outputStream;

    private BdpFileDescriptor descriptor;

    public HdfsSshFile(final FileSystem hdfs,
            final BuilderDdpFileDescriptorAbstract builder,
            final String userName, final String fileName) {
        if (hdfs == null) {
            throw new IllegalArgumentException("Hdfs can not be null");
        }
        if (fileName == null) {
            throw new IllegalArgumentException("fileName can not be null");
        }

        if (fileName.length() == 0) {
            throw new IllegalArgumentException("fileName can not be empty");
        } else if (fileName.charAt(0) != '/') {
            throw new IllegalArgumentException(
                    "fileName must be an absolute path");
        }
        this.descriptor = builder.build(userName, fileName);

        this.fileName = fileName;
        this.path = new Path(fileName);
        this.hdfs = hdfs;
    }

    public String getAbsolutePath() {
        // strip the last '/' if necessary
        String fullName = fileName;
        int filelen = fullName.length();
        if ((filelen != 1) && (fullName.charAt(filelen - 1) == '/')) {
            fullName = fullName.substring(0, filelen - 1);
        }

        return fullName;
    }

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

    public boolean isDirectory() {
        try {
            return this.hdfs.getFileStatus(path).isDir();
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
            throw new RuntimeException(e);
        }
    }

    public boolean isFile() {
        try {
            return !this.hdfs.getFileStatus(path).isDir();
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    public boolean doesExist() {
        try {
            return hdfs.exists(path);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    public boolean isReadable() {
        return descriptor.isReadable();
    }

    public boolean isWritable() {
        return descriptor.isWritable();
    }

    public boolean isExecutable() {
        // TODO Auto-generated method stub
        return false;
    }

    public boolean isRemovable() {
        // TODO Auto-generated method stub
        return false;
    }

    public SshFile getParentFile() {
        // TODO Auto-generated method stub
        return null;
    }

    public long getLastModified() {
        try {
            FileStatus fileStatus = hdfs.getFileStatus(path);
            return fileStatus.getModificationTime();
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    public boolean setLastModified(long time) {
        try {
            hdfs.setTimes(path, time, time);
            return true;
        } catch (Exception e) {
            return false;
        }
    }

    public long getSize() {
        try {
            FileStatus fileStatus = hdfs.getFileStatus(path);
            return fileStatus.getLen();
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    public boolean mkdir() {
        boolean retVal = false;
        if (isWritable()) {
            try {
                retVal = hdfs.mkdirs(path);
            } catch (IOException e) {
                throw new RuntimeException(e);
            }
        }
        return retVal;
    }

    public boolean delete() {
        try {
            return hdfs.delete(path, true);
        } catch (Exception e) {
            return false;
        }
    }

    public boolean create() throws IOException {
        outputStream = hdfs.create(path);
        return true;
    }

    public void truncate() throws IOException {
        // TODO Auto-generated method stub

    }

    public boolean move(SshFile destination) {
        return false;
    }

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
            virtualFiles[i] = new HdfsSshFile(hdfs, builder, userName, fileName);
        }

        return Collections.unmodifiableList(Arrays.asList(virtualFiles));
    }

    public OutputStream createOutputStream(long offset) throws IOException {
        if (offset > 0) {
            throw new RuntimeException();
        }
        if (this.outputStream == null) {
            create();
        }

        if (this.descriptor.isSerializable()) {
            LzopCodec codec = new LzopCodec();
            codec.setConf(hdfs.getConf());
            outputStream = codec.createOutputStream(outputStream);
        }

        BdpCompresion compression = descriptor.getCompresion();

        if (compression != null) {
            try {
                @SuppressWarnings("unchecked")
                Class<ParserAbstract> klass = (Class<ParserAbstract>) Class
                        .forName(compression.getClassName());

                Constructor<ParserAbstract> constructor = klass.getConstructor(
                        String.class, String.class);

                ParserAbstract parser = constructor.newInstance(
                        compression.getPattern(), compression.getAttr());
                outputStream = new ProtoBufOutStream(outputStream, parser);
            } catch (Exception e) {
                throw new RuntimeException();
            }
        }
        return outputStream;

    }

    public InputStream createInputStream(long offset) throws IOException {
        if (offset > 0) {
            throw new RuntimeException();
        }
        return hdfs.open(path);
    }

    public void handleClose() throws IOException {
        // TODO Auto-generated method stub

    }

}
