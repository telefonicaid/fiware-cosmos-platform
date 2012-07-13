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
import es.tid.cosmos.mobility.data.generated.MobProtocol.CellGroup;
import es.tid.cosmos.mobility.parsing.CellGroupParser;

/**
 *
 * @author dmicol
 */
public abstract class CellGroupsCatalogue {
    public static List<CellGroup> load(Path input, MobilityConfiguration conf)
            throws IOException {
        FSDataInputStream in = null;
        Reader reader = null;
        try {
            FileSystem fs = FileSystem.get(conf);
            in = fs.open(input);
            reader = new InputStreamReader(in);
            return load(reader, conf.getDataSeparator());
        } catch (Exception ex) {
            Logger.get(CellGroupsCatalogue.class).fatal(ex);
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

    public static List<CellGroup> load(Reader input, String separator)
            throws IOException {
        List<CellGroup> cells = new LinkedList<CellGroup>();
        BufferedReader br = new BufferedReader(input);
        String line;
        while ((line = br.readLine()) != null) {
            CellGroup cellGroup = new CellGroupParser(line, separator).parse();
            cells.add(cellGroup);
        }
        return cells;
    }
    
    public static List<CellGroup> filter(List<CellGroup> cells, long cellId) {
        List<CellGroup> filteredCells = new LinkedList<CellGroup>();
        for (CellGroup cellGroup : cells) {
            if (cellGroup.getCellId() == cellId) {
                filteredCells.add(cellGroup);
            }
        }
        return filteredCells;
    }
}
