package es.tid.cosmos.mobility.util;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.HashSet;
import java.util.Set;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FSDataInputStream;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;

/**
 *
 * @author dmicol
 */
public final class CellCatalogue {
    private static Set<Long> cellCatalogue = null;
    
    public static Set<Long> get(Configuration conf) {
        if (cellCatalogue == null) {
            load(conf);
        }
        return cellCatalogue;
    }
    
    private static void load(Configuration conf) {
        try {
            Path input = new Path(conf.get("CELL_CATALOGUE_PATH"));
            
            FileSystem fs = FileSystem.get(conf);
            FSDataInputStream in = fs.open(input);
            BufferedReader br = new BufferedReader(new InputStreamReader(in));
            cellCatalogue = new HashSet<Long>();
            String line;
            while ((line = br.readLine()) != null) {
                String[] columns = line.split("\\|");
                cellCatalogue.add(Long.decode(columns[0]));
            }
            in.close();
        } catch (IOException ex) {
            ex.printStackTrace(System.err);
        } catch (NullPointerException ex) {
            ex.printStackTrace(System.err);
        }
    }
}
