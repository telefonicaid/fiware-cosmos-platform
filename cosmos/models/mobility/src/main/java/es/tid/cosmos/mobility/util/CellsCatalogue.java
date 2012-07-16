package es.tid.cosmos.mobility.util;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.Reader;
import java.util.LinkedList;
import java.util.List;

import org.apache.hadoop.fs.FSDataInputStream;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;

import es.tid.cosmos.base.util.Logger;
import es.tid.cosmos.mobility.conf.MobilityConfiguration;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cell;
import es.tid.cosmos.mobility.parsing.CellParser;

/**
 *
 * @author dmicol
 */
public abstract class CellsCatalogue {
    public static List<Cell> load(Path input, MobilityConfiguration conf)
            throws IOException {
        FSDataInputStream in = null;
        Reader reader = null;
        try {
            FileSystem fs = FileSystem.get(conf);
            in = fs.open(input);
            reader = new InputStreamReader(in);
            final String separator = conf.getDataSeparator();
            return load(reader, separator);
        } catch (Exception ex) {
            Logger.get(CellsCatalogue.class).fatal(ex);
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

    public static List<Cell> load(Reader input, String separator)
            throws IOException {
        List<Cell> cells = new LinkedList<Cell>();
        BufferedReader br = new BufferedReader(input);
        String line;
        while ((line = br.readLine()) != null) {
            Cell cell = new CellParser(line, separator).parse();
            cells.add(cell);
        }
        return cells;
    }

    public static List<Cell> filter(List<Cell> cells, long cellId) {
        List<Cell> filteredCells = new LinkedList<Cell>();
        for (Cell cell : cells) {
            if (cell.getCellId() == cellId) {
                filteredCells.add(cell);
            }
        }
        return filteredCells;
    }
}
