package es.tid.bdp.sftp.server.filesystem.hadoop;

import java.io.IOException;
import java.lang.reflect.Constructor;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.sshd.server.FileSystemView;
import org.apache.sshd.server.SshFile;
import org.apache.sshd.server.filesystem.NativeFileSystemFactory;
import org.apache.sshd.server.filesystem.NativeFileSystemView;
import org.apache.sshd.server.filesystem.NativeSshFile;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import es.tid.bdp.utils.BuilderDdpFileDescriptorAbstract;
import es.tid.bdp.utils.BuilderDdpFileDescriptorMongo;
import es.tid.bdp.utils.BuilderDdpFileDescriptorTextAllow;
import es.tid.bdp.utils.PropertiesPlaceHolder;
import es.tid.bdp.utils.data.BdpFileDescriptor;
import es.tid.bdp.utils.parse.ParserAbstract;

public class HdfsFileSystemView implements FileSystemView {

    private final Logger LOG = LoggerFactory
            .getLogger(NativeFileSystemView.class);

    private final static String FS_DEFAULT_NAME = "fs.default.name";
    private static final String DESCRIPTOR_BUILDER_CLASS = "filesystem.descriptor.class";

    private String userName;
    private FileSystem hdfsSrc;

    private String currDir;

    private BuilderDdpFileDescriptorAbstract builder;

    private boolean caseInsensitive = false;

    /**
     * Constructor - internal do not use directly, use
     * {@link NativeFileSystemFactory} instead
     */
    protected HdfsFileSystemView(String userName) {
        this(userName, false);
    }

    /**
     * Constructor - internal do not use directly, use
     * {@link NativeFileSystemFactory} instead
     */
    public HdfsFileSystemView(String userName, boolean caseInsensitive) {
        if (userName == null) {
            throw new IllegalArgumentException("user can not be null");
        }

        this.caseInsensitive = caseInsensitive;

        this.currDir = "/";
        this.userName = userName;

        try {

            PropertiesPlaceHolder properties = PropertiesPlaceHolder
                    .getInstance();

            Configuration conf = new Configuration();
            conf.set(FS_DEFAULT_NAME, properties.getProperty(FS_DEFAULT_NAME));

            conf.set(
                    "io.compression.codecs",
                    "org.apache.hadoop.io.compress.GzipCodec,org.apache.hadoop.io.compress.DefaultCodec,com.hadoop.compression.lzo.LzoCodec,com.hadoop.compression.lzo.LzopCodec,org.apache.hadoop.io.compress.BZip2Codec");

            conf.set("io.compression.codec.lzo.class",
                    "com.hadoop.compression.lzo.LzoCodec");

            hdfsSrc = FileSystem.get(conf);

            @SuppressWarnings("unchecked")
            Class<BuilderDdpFileDescriptorAbstract> klass = (Class<BuilderDdpFileDescriptorAbstract>) Class
                    .forName(properties.getProperty(DESCRIPTOR_BUILDER_CLASS));

            Constructor<BuilderDdpFileDescriptorAbstract> constructor = klass
                    .getConstructor(PropertiesPlaceHolder.class);

            builder = constructor.newInstance(properties);
        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }

        // add last '/' if necessary
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

        return new HdfsSshFile(hdfsSrc, builder, userName, physicalName);
    }
}
