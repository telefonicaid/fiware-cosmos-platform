package es.tid.bdp.sftp.io;

import java.io.IOException;
import java.io.OutputStream;

import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.SequenceFile.Writer;

import com.google.protobuf.Message;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

public class ProtoBufCrdsOutStream extends OutputStream {

    private static final String LINE_SEPARATOR = "\n";

    private StringBuilder linea;
    private Writer writer;
    private Parser parser;

    /**
     * Constructor. Into the construct it loads the configuration from the
     * properties holder. It prepares the schema for generating the record and
     * the structure for parsing the line
     * 
     * @param out
     *            a stream for writing output.
     * @throws IOException
     * @throws IllegalAccessException
     * @throws InstantiationException
     * @throws ClassNotFoundException
     * @throws NoSuchFieldException
     * @throws SecurityException
     */
    public ProtoBufCrdsOutStream(Writer writer) throws IOException {        
        this.parser = new Parser();
        this.writer = writer;
        this.linea = new StringBuilder();

    }

    /*
     * This method is not implemented
     */
    @Override
    public void write(int arg0) throws IOException {
        throw new RuntimeException("method not implemented");
    }

    /*
     * (non-Javadoc)
     * 
     * @see java.io.OutputStream#write(byte[], int, int)
     */
    @Override
    public void write(byte[] buffer, int off, int len) throws IOException {
        linea.append(new String(buffer), off, len);
        int offset = linea.indexOf(LINE_SEPARATOR);
        while (offset != -1) {
            String returnValue = linea.substring(0, offset);
            linea.delete(0, offset + 1);
            offset = linea.indexOf(LINE_SEPARATOR);
            try {
                ProtobufWritable<Message> record = parser.parseLine(returnValue);

                writer.append(NullWritable.get(), record);

            } catch (Exception e) {
                // TODO: handle exception
                System.err.println("error: " + returnValue);
                // System.in.read();
                e.printStackTrace();
            }
        }
    }

    /*
     * (non-Javadoc)
     * 
     * @see java.io.OutputStream#close()
     */
    @Override
    public void close() throws IOException {
        if (writer != null) {
            writer.close();
        }
    }
}
