package es.tid.cosmos.mobility.util;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.LinkedList;
import java.util.List;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FSDataInputStream;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;

import es.tid.cosmos.mobility.data.MobProtocol.Cell;
import es.tid.cosmos.mobility.parsing.CellParser;

/**
 *
 * @author dmicol
 */
public abstract class CellsCatalogue {
    public static List<Cell> load(Path input, Configuration conf)
            throws IOException {
        FSDataInputStream in = null;
        BufferedReader br = null;
        try {
            FileSystem fs = FileSystem.get(conf);
            in = fs.open(input);
            br = new BufferedReader(new InputStreamReader(in));
            List<Cell> cells = new LinkedList<Cell>();
            String line;
            while ((line = br.readLine()) != null) {
                Cell cell = new CellParser(line).parse();
                cells.add(cell);
            }
            return cells;
        } catch (Exception ex) {
            Logger.get().fatal(ex);
            throw new IOException(ex);
        } finally {
            if (br != null) {
                try {
                    br.close();
                } catch (IOException ex) {
                }
            }
            if (in != null) {
                try {
                    in.close();
                } catch (IOException ex) {
                }
            }
        }
    }
}
