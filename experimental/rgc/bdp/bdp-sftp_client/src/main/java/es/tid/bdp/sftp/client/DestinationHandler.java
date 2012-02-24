package es.tid.bdp.sftp.client;

import java.io.IOException;
import java.io.OutputStream;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.SequenceFile;
import org.apache.hadoop.io.SequenceFile.Writer;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.bdp.sftp.io.ProtoBufOutStream;
import es.tid.bdp.utils.PropertiesPlaceHolder;

public class DestinationHandler {
    Configuration confDst;
    FileSystem hdfsDst;
    private boolean overwrite;

    private final static String FS_DEFAULT_NAME = "fs.default.name";
    private static final String IO_OUPUT_OVERWRITE = "io.output.overwrite";

    public DestinationHandler() throws IOException {
        PropertiesPlaceHolder properties = PropertiesPlaceHolder.getInstance();

        this.overwrite = properties.getPropertyBool(IO_OUPUT_OVERWRITE);

        confDst = new Configuration();
        confDst.set(FS_DEFAULT_NAME, properties.getProperty(FS_DEFAULT_NAME));
        hdfsDst = FileSystem.get(confDst);

    }

    /**
     * 
     * @param path
     * @return
     * @throws IOException
     */
    public OutputStream getOutputStream(String path) throws IOException {
System.out.println("dsfgsdfgsdfgs!" + path);
        Path outputFile = new Path(path);
        if (!overwrite && hdfsDst.exists(outputFile)) {
            throw new IllegalArgumentException();
        }
        Writer writer = SequenceFile.createWriter(hdfsDst, confDst, outputFile,
                NullWritable.class, ProtobufWritable.class);

        return new ProtoBufOutStream(writer);

    }

}
