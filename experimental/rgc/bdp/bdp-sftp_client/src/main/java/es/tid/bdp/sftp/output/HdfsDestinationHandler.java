package es.tid.bdp.sftp.output;

import java.io.IOException;
import java.io.OutputStream;

import javax.annotation.PostConstruct;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;

import com.hadoop.compression.lzo.LzopCodec;

import es.tid.bdp.utils.PropertiesPlaceHolder;
/**
 * 
 * @author rgc
 *
 */
public class HdfsDestinationHandler implements DestinationHandler {
    
    private static final Log LOG = LogFactory.getLog(LzopCodec.class);

    private Configuration confDst;
    private FileSystem hdfsDst;
    private boolean overwrite;
    private boolean lzoCompression;

    private final static String FS_DEFAULT_NAME = "fs.default.name";
    private static final String IO_OUPUT_OVERWRITE = "io.output.overwrite";
    private static final String IO_OUPUT_COMPRESSION = "io.output.compression";

    /**
     * Constructor
     */
    public HdfsDestinationHandler() {
        postConstructor();
    }

    /**
     * Postconstructor in where it reads the configuration and injects the
     * resources of the class, it is doing in this phase to facilitate the
     * generation the unit test
     */
    @PostConstruct
    private void postConstructor() {
        try {
            PropertiesPlaceHolder properties = PropertiesPlaceHolder
                    .getInstance();

            this.overwrite = properties.getPropertyBool(IO_OUPUT_OVERWRITE);
            this.lzoCompression = properties
                    .getPropertyBool(IO_OUPUT_COMPRESSION);

            confDst = new Configuration();
            confDst.set(FS_DEFAULT_NAME,
                    properties.getProperty(FS_DEFAULT_NAME));
            hdfsDst = FileSystem.get(confDst);
            
            LOG.debug("Properties have been reading.");
                      
        } catch (Exception e) {
            // TODO: handle exception
            LOG.error("Error reading the properties.", e);
            e.printStackTrace();
        }
    }

    /*
     * (non-Javadoc)
     * 
     * @see
     * es.tid.bdp.sftp.output.DestinationHandler#getOutputStream(java.lang.String
     * )
     */
    @Override
    public OutputStream getOutputStream(String path) throws IOException {
        Path outputFile = new Path(path);
        if (!overwrite && hdfsDst.exists(outputFile)) {
            throw new IllegalArgumentException();
        }
        OutputStream outputStream = hdfsDst.create(outputFile, true);
        if (lzoCompression) {
            LzopCodec codec = new LzopCodec();
            codec.setConf(confDst);
            outputStream = codec.createOutputStream(outputStream);
            LOG.debug("Lzo compression has been configurated.");

        }
        return new ProtoBufOutStream(outputStream);
    }

}