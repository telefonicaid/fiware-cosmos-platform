package es.tid.o2aaic.ipm;

import java.io.*;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;

import es.tid.cosmos.base.util.Logger;

/**
 *
 * @author dmicol
 */
public class IpmMain extends Configured implements Tool {
    private static final String INET_TYPE = "inet";
    private static final String CRM_TYPE = "crm";
    
    @Override
    public int run(String[] args) throws Exception {
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
        
        return 0;
    }
    
    public static void main(String[] args) throws Exception {
        try {
            int res = ToolRunner.run(new Configuration(), new IpmMain(), args);
            if (res != 0) {
                throw new Exception("Unknown error");
            }
        } catch (Exception ex) {
            Logger.get(IpmMain.class).fatal(ex);
            throw ex;
        }
    }
}
