package es.tid.bdp.sftp.output;

import java.io.IOException;
import java.io.OutputStream;
import java.lang.reflect.Constructor;

import javax.annotation.PostConstruct;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;

import com.hadoop.compression.lzo.LzopCodec;

import es.tid.bdp.utils.PropertiesPlaceHolder;
import es.tid.bdp.utils.io.output.ProtoBufOutStream;
import es.tid.bdp.utils.parse.ParserAbstract;

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
    private ParserAbstract parser;

    private final static String FS_DEFAULT_NAME = "fs.default.name";
    private static final String IO_OUPUT_OVERWRITE = "io.output.overwrite";
    private static final String IO_OUPUT_COMPRESSION = "io.output.compression";
    private static final String INPUT_PARSER_CLASS = "input.parser.class";
    private static final String INPUT_ATTRS_REGEX = "input.attrs.regex";
    private static final String INPUT_ATTRS_DESC = "input.attrs.desc";

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

            overwrite = properties.getPropertyBool(IO_OUPUT_OVERWRITE);
            lzoCompression = properties
                    .getPropertyBool(IO_OUPUT_COMPRESSION);

            confDst = new Configuration();
            confDst.set(FS_DEFAULT_NAME,
                    properties.getProperty(FS_DEFAULT_NAME));
            hdfsDst = FileSystem.get(confDst);

            LOG.debug("Properties have been reading.");

            @SuppressWarnings("unchecked")
            Class<ParserAbstract> klass = (Class<ParserAbstract>) Class
                    .forName(properties.getProperty(INPUT_PARSER_CLASS));

            Constructor<ParserAbstract> constructor = klass.getConstructor(
                    String.class, String.class);

            parser = constructor.newInstance(properties.getProperty(INPUT_ATTRS_REGEX),
                    properties.getProperty(INPUT_ATTRS_DESC));

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

            confDst.set(
                    "io.compression.codecs",
                    "org.apache.hadoop.io.compress.GzipCodec,org.apache.hadoop.io.compress.DefaultCodec,com.hadoop.compression.lzo.LzoCodec,com.hadoop.compression.lzo.LzopCodec,org.apache.hadoop.io.compress.BZip2Codec");

            confDst.set("io.compression.codec.lzo.class",
                    "com.hadoop.compression.lzo.LzoCodec");
            LzopCodec codec = new LzopCodec();
            codec.setConf(confDst);
            outputStream = codec.createOutputStream(outputStream);

            LOG.debug("Lzo compression has been configurated.");

        }
        return new ProtoBufOutStream(outputStream, parser);
    }
}