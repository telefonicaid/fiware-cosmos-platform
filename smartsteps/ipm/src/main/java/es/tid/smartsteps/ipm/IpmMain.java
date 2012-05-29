package es.tid.smartsteps.ipm;

import java.io.*;

/**
 *
 * @author dmicol
 */
public class IpmMain {
    private static final String INET_TYPE = "inet";
    private static final String CRM_TYPE = "crm";
    
    public static void main(String[] args) throws Exception {
        if (args.length != 3) {
            throw new IllegalArgumentException("Invalid number of arguments");
        }
        
        final String input = args[0];
        final String output = args[1];
        final String type = args[2];
        RawToItpmConverter converter;
        if (type.equals(INET_TYPE)) {
             converter = new InetRawToIpmConverter();
        } else if (type.equals(CRM_TYPE)) {
            converter = new CrmRawToIpmConverter();
        } else {
            throw new IllegalArgumentException("Invalid data type: " + type);
        }
        
        BufferedReader reader = null;
        BufferedWriter writer = null;
        try {
            reader = new BufferedReader(new FileReader(input));
            writer = new BufferedWriter(new FileWriter(output));
            String line;
            while ((line = reader.readLine()) != null) {
                writer.write(converter.convert(line));
            }
        } catch (IOException ex) {
            throw ex;
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
