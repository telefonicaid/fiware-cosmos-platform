package es.tid.ps.translation;

import java.io.InputStream;
import java.io.IOException;
import java.util.Properties;

/**
 * Class that implements the configuration of a translator, based on Java
 * properties.
 *
 * @author dmicol
 */
public class Configuration {
    /**
     * Field name in the configuration file of the type field.
     */
    private static final String TYPE_FIELD_NAME = "type";

    /**
     * Field name in the configuration file of the delimiter field.
     */
    private static final String DELIMITER_FIELD_NAME = "delimiter";

    /**
     * Field name in the configuration file of the field assignments field.
     */
    private static final String FIELD_ASSIGNMENTS_FIELD_NAME =
            "field_assignments";

    /**
     * Field name in the configuration file of the output path field.
     */
    private static final String OUTPUT_PATH_FIELD_NAME = "output_path";

    /**
     * Delimiter of the field assignments in the configuration file.
     */
    private static final String FIELD_ASSIGNMENTS_DELIMITER = ",";

    /**
     * Type of information to be processed.
     */
    public enum Type {
        /**
         * Invalid type, used to initialize.
         */
        INVALID,
        /**
         * Call-Detail Record.
         */
        CDR,
        /**
         * Web log navigation info.
         */
        WEB_NAVIGATION
    }

    /**
     * Type of the data of the input file being processed.
     */
    private Type type;

    /**
     * Value delimiter of the input file being processed.
     */
    private String delimiter;

    /**
     * Mapping between the input data columns and the proto buffer message
     * fields. The length of this array will be the number of fields of the
     * corresponding proto buffer, and each element will store the index of the
     * column in the input data that should go into that field (0-based). So if
     * the third column of the input data should go into the second field of
     * the proto buffer, then fieldAssignments[1] = 2.
     */
    private int[] fieldAssignments;

    /**
     * HDFS path to write the output to.
     */
    private String outputPath;

    /**
     * Determines whether the configuration has been loaded.
     */
    private boolean isLoaded;

    public Type getType() {
        return this.type;
    }

    public String getDelimiter() {
        return this.delimiter;
    }

    public int[] getFieldAssignments() {
        return this.fieldAssignments;
    }

    public String getOutputPath() {
        return this.outputPath;
    }

    public boolean getIsLoaded() {
        return this.isLoaded;
    }

    /**
     * Overloaded constructor.
     *
     * @param configurationFile
     *            The file to read the configuration from.
     */
    public Configuration(String configurationFile) {
        this.loadConfiguration(configurationFile);
    }

    /**
     * Loads the configuration.
     *
     * @param configurationFile
     *            The path to the configuration file.
     * @return Whether it was loaded successfully.
     */
    public boolean loadConfiguration(String configurationFile) {
        InputStream input = null;
        try {
            input = getClass().getResourceAsStream(configurationFile);
            Properties properties = new Properties();
            properties.load(input);

            this.type = Type.valueOf(properties.getProperty(TYPE_FIELD_NAME));
            if (this.type == Type.INVALID) {
                return false;
            }
            this.delimiter = properties.getProperty(DELIMITER_FIELD_NAME);
            this.fieldAssignments = this.splitAndParseNumberList(properties
                    .getProperty(FIELD_ASSIGNMENTS_FIELD_NAME));
            if ((this.fieldAssignments == null)
                    || (this.fieldAssignments.length == 0)) {
                return false;
            }
            this.outputPath = properties.getProperty(OUTPUT_PATH_FIELD_NAME);
            this.isLoaded = true;
        } catch (IOException e1) {
            System.err.println(e1.getMessage());
            return false;
        } finally {
            try {
                if (input != null) {
                    input.close();
                }
            } catch (IOException e2) {
                System.err.println(e2.getMessage());
                return false;
            }
        }
        return true;
    }

    /**
     * Splits a text line and parses its corresponding values into an integer
     * array.
     *
     * @param text
     *            The aforementioned text line.
     * @return The array of integers.
     */
    private int[] splitAndParseNumberList(String text) {
        String[] textValues = text.split(FIELD_ASSIGNMENTS_DELIMITER);
        int[] numericalValues = new int[textValues.length];
        for (int i = 0; i < textValues.length; ++i) {
            try {
            numericalValues[i] = Integer.parseInt(textValues[i]);
            } catch (NumberFormatException e) {
                System.err.println(e.getMessage());
            }
        }
        return numericalValues;
    }
}
