package es.tid.cosmos.mobility.util;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.Reader;
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
        Reader reader = null;
        try {
            FileSystem fs = FileSystem.get(conf);
            in = fs.open(input);
            reader = new InputStreamReader(in);
            return load(reader);
        } catch (Exception ex) {
            Logger.get().fatal(ex);
            throw new IOException(ex);
        } finally {
            if (reader != null) {
                try {
                    reader.close();
                } catch (IOException ignored) {
                }
            }
            if (in != null) {
                try {
                    in.close();
                } catch (IOException ignored) {
                }
            }
        }
    }

    public static List<Cell> load(Reader input) throws IOException {
        List<Cell> cells = new LinkedList<Cell>();
        BufferedReader br = new BufferedReader(input);
        String line;
        while ((line = br.readLine()) != null) {
            Cell cell = new CellParser(line).parse();
            cells.add(cell);
        }
        return cells;
    }
}
