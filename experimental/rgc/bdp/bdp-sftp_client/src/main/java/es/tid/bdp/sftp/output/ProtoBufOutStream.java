package es.tid.bdp.sftp.output;

import java.io.IOException;
import java.io.OutputStream;

import com.google.protobuf.Message;
import com.twitter.elephantbird.mapreduce.io.ProtobufBlockWriter;

import es.tid.bdp.utils.PropertiesPlaceHolder;
import es.tid.bdp.utils.parse.ParserAbstract;

public class ProtoBufOutStream extends OutputStream {

    private static final String LINE_SEPARATOR = "\n";
    private static final String INPUT_PARSER_CLASS = "input.parser.class";

    private StringBuilder linea;
    private ParserAbstract parser;

    private ProtobufBlockWriter<Message> writer;

    /**
     * Constructor. Into the construct it loads the configuration from the
     * properties holder. It prepares the schema for generating the record and
     * the structure for parsing the line
     * 
     * @param out
     *            a stream for writing output.
     * @throws IOException
     */
    public ProtoBufOutStream(OutputStream outputStream) throws IOException {
        PropertiesPlaceHolder properties = PropertiesPlaceHolder.getInstance();
        try {
            @SuppressWarnings("unchecked")
            Class<ParserAbstract> klass = (Class<ParserAbstract>) Class
                    .forName(properties.getProperty(INPUT_PARSER_CLASS));

            this.parser = klass.newInstance();
        } catch (Exception e) {
            // TODO: handle exception
            e.printStackTrace();
        }
        this.writer = new ProtobufBlockWriter<Message>(outputStream,
                Message.class);

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
                Message record = parser.parseLine(returnValue);

                writer.write(record);

            } catch (Exception e) {
                // TODO: handle exception
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
            writer.finish();
            writer.close();
        }
    }
}
