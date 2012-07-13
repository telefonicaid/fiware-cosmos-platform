package es.tid.smartsteps.ipm;

import java.io.*;
import java.nio.charset.Charset;

/**
 *
 * @author dmicol
 */
public final class IpmMain {
    private static final String INET_TYPE = "inet";
    private static final String CRM_TYPE = "crm";
    private static final int EXPECTED_ARGC = 3;
    private static final String PROPERTY_CHARSET = "ipm.charset";
    private static final String DEFAULT_CHARSET = "UTF-8";

    private IpmMain() {
    }
    
    public static void main(String[] args) throws IOException, ParseException {
        if (args.length != EXPECTED_ARGC) {
            throw new IllegalArgumentException("Invalid number of arguments.\n"
                    + "Usage: ipm.jar input output <inet|crm>");
        }
        
        final String inputFilename = args[0];
        final String outputFilename = args[1];
        final String type = args[2];
        String charset = System.getProperty(PROPERTY_CHARSET);
        if (charset == null) {
            charset = DEFAULT_CHARSET;
        }
        RawToIpmConverter converter;
        if (type.equals(INET_TYPE)) {
            converter = new InetRawToIpmConverter("|", Charset.forName(charset));
        } else if (type.equals(CRM_TYPE)) {
            converter = new CrmRawToIpmConverter("|", Charset.forName(charset));
        } else {
            throw new IllegalArgumentException("Invalid data type: " + type);
        }

        InputStream input;
        OutputStream output;
        BufferedReader reader = null;
        BufferedWriter writer = null;
        try {
            input = new FileInputStream(inputFilename);
            output = new FileOutputStream(outputFilename);
            converter.convert(input, output);
        } finally {
            if (writer != null) {
                writer.close();
            }
            if (reader != null) {
                reader.close();
            }
        }
    }
}
