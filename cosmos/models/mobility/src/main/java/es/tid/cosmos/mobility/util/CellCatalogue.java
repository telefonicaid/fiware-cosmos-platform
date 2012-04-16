package es.tid.cosmos.mobility.util;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.HashMap;
import java.util.Map;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FSDataInputStream;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.log4j.Logger;

import es.tid.cosmos.mobility.MobilityMain;
import es.tid.cosmos.mobility.data.MobProtocol.Cell;
import es.tid.cosmos.mobility.parsing.CellParser;

/**
 *
 * @author dmicol
 */
public class CellCatalogue {
    private static final Logger LOG = Logger.getLogger(MobilityMain.class);
    
    private Map<Long, Cell> cellCatalogue;
    
    public CellCatalogue(Configuration conf) throws IOException {
        this.load(conf);
    }
    
    public boolean contains(long id) {
        return this.cellCatalogue.containsKey(id);
    }
    
    public Cell get(long id) {
        return this.cellCatalogue.get(id);
    }
    
    private void load(Configuration conf) throws IOException {
        FSDataInputStream in = null;
        BufferedReader br = null;
        try {
            Path input = new Path(conf.get("CELL_CATALOGUE_PATH"));
            
            FileSystem fs = FileSystem.get(conf);
            in = fs.open(input);
            br = new BufferedReader(new InputStreamReader(in));
            this.cellCatalogue = new HashMap<Long, Cell>();
            String line;
            while ((line = br.readLine()) != null) {
                Cell cell = new CellParser(line).parse();
                // TODO: double-check if this should be cell or place ID
                this.cellCatalogue.put(cell.getPlaceId(), cell);
            }
        } catch (Exception ex) {
            this.cellCatalogue = null;
            LOG.fatal(ex);
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
