package es.tid.bdp.sftp.client;

import java.io.File;
import java.io.IOException;
import java.io.OutputStream;
import java.util.List;

import javax.annotation.PostConstruct;

import es.tid.bdp.sftp.client.input.SftpSourceHandler;
import es.tid.bdp.sftp.client.input.SourceHandler;
import es.tid.bdp.sftp.output.DestinationHandler;
import es.tid.bdp.sftp.output.HdfsDestinationHandler;
import es.tid.bdp.utils.PropertiesPlaceHolder;

/**
 * This class control the process for reading a file from a source and sending
 * it a destinations. It generates a destination path from the data reading in
 * the configuration and applying a regular expresion to change the original
 * name.
 * 
 * @author rgc
 * 
 */
public class ConnectionHandler {

    private static final String IO_INPUT_PATH = "io.input.path";
    private static final String IO_INPUT_REGEX = "io.input.regex";
    private static final String IO_OUPUT_PATH = "io.output.path";
    private static final String IO_OUPUT_REPLACE_REGEX = "io.output.replace.regex";
    private static final String IO_OUPUT_REPLACE_REPLACEMENT = "io.output.replace.replacement";

    private SourceHandler client;
    private DestinationHandler dest;
    private String path;
    private String regex;
    private String outPath;
    private String outNameRegex;
    private String outNameReplace;

    /**
     * Constructor
     */
    public ConnectionHandler() {
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
            this.path = properties.getProperty(IO_INPUT_PATH);
            this.regex = properties.getProperty(IO_INPUT_REGEX);
            this.outPath = properties.getProperty(IO_OUPUT_PATH);
            this.outNameRegex = properties.getProperty(IO_OUPUT_REPLACE_REGEX);
            this.outNameReplace = properties
                    .getProperty(IO_OUPUT_REPLACE_REPLACEMENT);

            client = new SftpSourceHandler();
            dest = new HdfsDestinationHandler();
        } catch (Exception e) {
            // TODO: handle exception
        }
    }

    /**
     * Method how orchestrates the operation to copy from the resource to the
     * destination
     * 
     * @throws IOException
     *             if the file from the source does not exist, is a directory
     *             rather than a regular file, or for some other reason cannot
     *             be opened for reading, or if is not possible to open the file
     *             in the destination
     */
    public void run() throws IOException {

        List<String> list = client.getFiles(path, regex);

        for (String inputPath : list) {
            String ouputPath = convertPath(inputPath);
            OutputStream output = dest.getOutputStream(ouputPath);
            client.copy(inputPath, output);
        }
    }

    /**
     * This method generates the path and the name of the file in the
     * destination
     * 
     * @param inputPath
     *            the absolute path of the file in the source
     * @return the absolute path of the file in the destination
     */
    private String convertPath(String inputPath) {

        File file = new File(inputPath);
        File outputFile = new File(this.outPath, file.getName().replace(
                this.outNameRegex, this.outNameReplace));

        return outputFile.getAbsolutePath();
    }

}
