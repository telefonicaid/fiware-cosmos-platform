package es.tid.bdp.mobility.mapreduce;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FSDataInputStream;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.log4j.Logger;

import es.tid.bdp.mobility.data.Cell;
import es.tid.bdp.mobility.data.CellCatalogue;
import es.tid.bdp.mobility.data.CellCatalogueFactory;
import es.tid.bdp.mobility.data.MobProtocol.Cdr;
import es.tid.bdp.mobility.data.MobProtocol.GLEvent;
import es.tid.bdp.mobility.parsing.ParserCdr;
import es.tid.bdp.mobility.parsing.ParserCell;
import es.tid.bdp.mobility.parsing.ParserFactory;

public class IndividualMobilityMapper extends Mapper<LongWritable, Text,
        LongWritable, ProtobufWritable<GLEvent>> {
    private static final Logger LOG = Logger.getLogger(
            IndividualMobilityMapper.class);
    
    // TODO: set this via a config file
    public static final String HDFS_CELL_CATALOGUE_PATH = "/data/cell.dat";
    
    private static final String CELL_PARSER = "DEFAULT";
    private static final String CDRS_PARSER = "DEFAULT";
    
    // TODO: this should be placed in the distributed cache
    private static boolean isCatalogLoaded;
    
    private CellCatalogue cellsCataloge;
    private LongWritable outputKey = new LongWritable();
    private ProtobufWritable<GLEvent> outputValue;

    public IndividualMobilityMapper() {
    }

    @Override
    protected void setup(final Context context) throws IOException,
                                                       InterruptedException {
        this.loadCellCatalogue(context.getConfiguration(),
                               HDFS_CELL_CATALOGUE_PATH);
        this.outputValue = ProtobufWritable.newInstance(GLEvent.class);
    }

    @Override
    protected void map(final LongWritable key, final Text value,
                       final Context context) throws IOException,
                                                     InterruptedException {
        final ParserCdr cdrParser = new ParserFactory().createNewCdrParser(
                CDRS_PARSER);
        Cdr cdr = cdrParser.parseCdrsLine(value.toString());

        this.outputKey.set(cdr.getUserId());
        GLEvent.Builder glEvent = GLEvent.newBuilder();
        glEvent.setUserId(cdr.getUserId());
        glEvent.setDate(cdr.getDate());
        glEvent.setTime(cdr.getTime());

        if (this.cellsCataloge != null
            && this.cellsCataloge.containsCell(cdr.getCellId())) {
            final Cell currentCell = this.cellsCataloge.getCell(
                    cdr.getCellId());
            glEvent.setPlaceId(currentCell.getGeoLocationLevel2());
        } else {
            glEvent.setPlaceId(0);
        }

        this.outputValue.set(glEvent.build());
        context.write(this.outputKey, this.outputValue);
    }

    private void loadCellCatalogue(Configuration conf,
                                   String hdfsFileLocation) {
        LOG.debug("Load Cell Catalogue from HDFS");

        this.cellsCataloge = CellCatalogueFactory.getInstance();
        if (isCatalogLoaded) {
            return;
        }

        FSDataInputStream in = null;
        BufferedReader br = null;
        try {
            FileSystem fs = FileSystem.get(conf);
            Path path = new Path(hdfsFileLocation);
            in = fs.open(path);
            br = new BufferedReader(new InputStreamReader(in));

            ParserCell cellParser = new ParserFactory().createNewCellParser(
                    CELL_PARSER);
            String line;
            while ((line = br.readLine()) != null) {
                final Cell cell = cellParser.parseCellLine(line);
                this.cellsCataloge.addCell(cell);
            }
            isCatalogLoaded = true;
        } catch (IOException ex) {
            LOG.debug("read from distributed cache: read length and instances");
        } catch (NullPointerException ex) {
            LOG.debug("read from distributed cache: read length and instances");
        } finally {
            try {
                if (in != null) {
                    in.close();
                }
            } catch (IOException ex) {
            }
        }
    }
}
