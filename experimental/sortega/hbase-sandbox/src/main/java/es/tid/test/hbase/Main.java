package es.tid.test.hbase;

import java.io.IOException;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.hbase.HBaseConfiguration;
import org.apache.hadoop.hbase.client.HTable;

/**
 *
 * @author sortega
 */
public class Main {
    
    public static void main(String argv[]) throws IOException {
        Configuration config = HBaseConfiguration.create();
        config.addResource(Main.class.getResource("config.xml"));
        
        HTable profileTable = new HTable(config, "individualprofile");
       
        printTopN(profileTable, "b9980e15c439fe91", 3);
        printTopN(profileTable, "b997ccc1d732f0be", 3);
    }

    private static void printTopN(HTable profileTable, final String userId, 
            long n) throws IOException {
        CategoryMapLookup lookup = new CategoryMapLookup(profileTable);
        CategoryMap categoryMap = lookup.getLastFor(userId);
        System.out.println(userId + "'s top " + n);
        for (String category: categoryMap.getTop(3)) {
            System.out.println("\t" + category);
        }
    }
    
}
