package es.tid.ps.translation;

import java.io.*;

import com.google.protobuf.Message;
import org.apache.hadoop.fs.*;

import es.tid.ps.protobuff.Cdr;
import es.tid.ps.protobuff.WebNavigation;

/**
 * This class implements a translator from an input source to protocol buffers.
 * It supports configuration files that define the input format as well as to
 * which protocol buffer to write to.
 * 
 * @author dmicol
 */
public final class Translator {
    /**
     * Configuration for the translator.
     */
    private Configuration configuration;

    /**
     * Handler to write to HDFS.
     */
    private FSDataOutputStream hdfsWriter;

    /**
     * Determines whether the configuration of the current object is
     * initialized.
     */
    private boolean isInitialized;

    /**
     * Default constructor.
     */
    public Translator() {
        this.isInitialized = false;
    }

    /**
     * Cleans up the current object. Mostly in charge of closing the HDFS
     * handler.
     */
    protected void finalize() {
        if (this.hdfsWriter != null) {
            try {
                this.hdfsWriter.close();
            } catch (IOException e) {
                System.err.println(e.getMessage());
            }
        }
    }

    /**
     * Initializes the current object's variables.
     * 
     * @param configurationFile
     */
    public void init(String configurationFile) throws IOException {
        this.isInitialized = false;

        // Load the translator configuration.
        this.configuration = new Configuration();
        this.configuration.loadConfiguration(configurationFile);

        // Load an HDFS writer.
        Path path = new Path(this.configuration.getOutputPath());
        FileSystem fs = FileSystem
                .get(new org.apache.hadoop.conf.Configuration());
        this.hdfsWriter = fs.create(path);
        this.isInitialized = true;
    }

    /**
     * Performs the translation from a file (e.g. batch).
     * 
     * @param inputFile
     *            The file that contains the input data.
     * @return Whether the operation succeeded.
     */
    public boolean translateFromFile(String inputFile) {
        BufferedReader reader = null;
        try {
            reader = new BufferedReader(new FileReader(inputFile));
            String rawText;
            while (reader.ready()) {
                rawText = reader.readLine();
                Message message = this.translateSingleRecord(rawText);
                // TODO(dmicol): writing one by one might not be optimal.
                this.writeMessage(message);
            }
        } catch (FileNotFoundException e1) {
            System.err.println("Could not find file.");
            return false;
        } catch (IOException e2) {
            System.err.println("Could not process file.");
            return false;
        } finally {
            try {
                if (reader != null) {
                    reader.close();
                }
            } catch (IOException e3) {
                System.err.println("Could not close file.");
                return false;
            }
        }
        return true;
    }

    /**
     * Performs the translation of a single record (e.g. streaming) to a
     * protocol buffer message.
     * 
     * @param text
     *            The record string version.
     * @return The generated message, and null if the operation failed.
     */
    public Message translateSingleRecord(String text) {
        assert this.isInitialized;

        String[] values = text.split(this.configuration.getDelimiter());
        switch (this.configuration.getType()) {
        case CDR:
            return this.createCDR(values);
        case WEB_NAVIGATION:
            return this.createWebNavigation(values);
        default:
            return null;
        }
    }

    /**
     * Writes a protocol buffer message to HDFS.
     * 
     * @param message
     *            The message to write.
     * @return Whether the write operation was successful.
     */
    private void writeMessage(Message message) throws IOException {
        assert this.isInitialized;

        message.writeTo(this.hdfsWriter);
    }

    /**
     * Creates a CDR protocol buffer.
     * 
     * @param values
     *            The set of values read from the data file.
     * @return The new CDR object.
     */
    private Cdr.CdrProto createCDR(String[] values) {
        int i = 0;
        Cdr.CdrProto cdr = Cdr.CdrProto
                .newBuilder()
                .setOriginNum(
                        values[this.configuration.getFieldAssignments()[i++]])
                .setDestinationNum(
                        values[this.configuration.getFieldAssignments()[i++]])
                .setDateTime(
                        values[this.configuration.getFieldAssignments()[i++]])
                .setCommType(
                        Cdr.CdrProto.Type.valueOf(values[this.configuration
                                .getFieldAssignments()[i++]])).build();
        return cdr;
    }

    /**
     * Creates a Web Navigation protocol buffer.
     * 
     * @param values
     *            The set of values read from the data file.
     * @return The new Web Navigation object.
     */
    private WebNavigation.WebNavigationProto createWebNavigation(String[] values) {
        int i = 0;
        WebNavigation.WebNavigationProto wn = WebNavigation.WebNavigationProto
                .newBuilder()
                .setUserId(
                        values[this.configuration.getFieldAssignments()[i++]])
                .setUrl(values[this.configuration.getFieldAssignments()[i++]])
                .setRequestTime(
                        values[this.configuration.getFieldAssignments()[i++]])
                .setUserAgent(
                        values[this.configuration.getFieldAssignments()[i++]])
                .setProtocol(
                        values[this.configuration.getFieldAssignments()[i++]])
                .setMethod(
                        values[this.configuration.getFieldAssignments()[i++]])
                .build();
        return wn;
    }
}
