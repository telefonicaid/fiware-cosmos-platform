package es.tid.bdp.utils.io.output;

import java.io.IOException;
import java.io.OutputStream;

import com.google.protobuf.Message;
import com.twitter.elephantbird.mapreduce.io.ProtobufBlockWriter;

import es.tid.bdp.utils.parse.ParserAbstract;

public class ProtoBufOutStream extends OutputStream {

    private static final String LINE_SEPARATOR = "\n";

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
     */
    public ProtoBufOutStream(OutputStream outputStream, ParserAbstract parser) {
        this.parser = parser;
        this.writer = new ProtobufBlockWriter<Message>(outputStream,
                Message.class);

        this.linea = new StringBuilder();
    }

    /*
     * (non-Javadoc)
     * 
     * @see java.io.OutputStream#write(int)
     */
    @Override
    public void write(int i) throws IOException {
        throw new RuntimeException("method id not implemented");
    }

    /*
     * (non-Javadoc)
     * 
     * @see java.io.OutputStream#write(byte[], int, int)
     */
    @Override
    public void write(byte[] buffer, int off, int len) throws IOException {
        String text1 = new String(buffer, 0, len, "UTF-8");
        linea.append(text1, off, text1.length());
        proccessBuffer();

    }

    /*
     * (non-Javadoc)
     * 
     * @see java.io.OutputStream#close()
     */
    @Override
    public void close() throws IOException {
        proccessBuffer();
        // Process last line
        write(linea.toString());
        writer.finish();
        writer.close();

    }

    /**
     * This method is for inject a mock writer for testing
     * 
     * @param writer
     *            mock of writer, for testing
     */
    protected void setWriter(ProtobufBlockWriter<Message> writer) {
        this.writer = writer;
    }

    /**
     * Process the buffer. Tokerizes the buffer by the separator, and then
     * writes the data
     */
    private void proccessBuffer() {
        int offset = linea.indexOf(LINE_SEPARATOR);
        while (offset != -1) {
            String returnValue = linea.substring(0, offset);
            linea.delete(0, offset + 1);
            offset = linea.indexOf(LINE_SEPARATOR);
            write(returnValue);
        }
    }

    /**
     * This method parses the line and writes en the buffer
     * 
     * @param string
     *            line with the data
     */
    private void write(String string) {
        try {
            Message record = parser.parseLine(string);
            writer.write(record);

        } catch (Exception e) {
            // TODO: handle exception
            System.out.println("error: " + string + " " + e.getMessage());
        }
    }
}
