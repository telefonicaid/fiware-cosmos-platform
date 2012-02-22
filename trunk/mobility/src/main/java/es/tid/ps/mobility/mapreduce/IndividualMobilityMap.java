package es.tid.ps.mobility.mapreduce;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
//import es.tid.analytics.mobility.core.data.Cdr;
import es.tid.ps.mobility.data.Cell;
import es.tid.ps.mobility.data.CellCatalogue;
import es.tid.ps.mobility.parsing.ParserCdr;
import es.tid.ps.mobility.parsing.ParserCell;
import es.tid.ps.mobility.parsing.ParserFactory;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FSDataInputStream;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.log4j.Logger;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;

import es.tid.ps.mobility.data.MobProtocol;
import es.tid.ps.mobility.data.MobProtocol.Cdr;
import es.tid.ps.mobility.data.MobProtocol.GLEvent;

public class IndividualMobilityMap extends Mapper<LongWritable, Text, LongWritable, ProtobufWritable<GLEvent>> {

    private static final Logger LOG = Logger.getLogger(IndividualMobilityMap.class);
    public static final String HDFS_CELL_CATALOGUE_PATH = "/data/cell.dat";
    private static final String CELL_PARSER = "DEFAULT";
    private static final String CDRS_PARSER = "DEFAULT";
    private CellCatalogue cellsCataloge;
    private static boolean loadCatalogue;
    private LongWritable outputKey = new LongWritable();
    private ProtobufWritable<GLEvent> outputValue;

    public IndividualMobilityMap() {
    }

    @Override
    protected void setup(final Context context) throws IOException,
            InterruptedException {
        loadCellCatalogue(context, HDFS_CELL_CATALOGUE_PATH);
        this.outputValue = ProtobufWritable.newInstance(GLEvent.class);
    }

    @Override
    protected void map(final LongWritable key, final Text value,
            final Context context) throws IOException, InterruptedException {
        final ParserCdr cdrParser = new ParserFactory().createNewCdrParser(CDRS_PARSER);
        Cdr cdr = cdrParser.parseCdrsLine(value.toString());

        this.outputKey.set(cdr.getUserId());
        GLEvent.Builder glEvent = GLEvent.newBuilder();
        glEvent.setUserId(cdr.getUserId());
        glEvent.setDate(cdr.getDate());
        glEvent.setTime(cdr.getTime());
        
        if (this.cellsCataloge != null
                && this.cellsCataloge.containsCell(cdr.getCellId())) {
            final Cell currentCell = this.cellsCataloge.getCell(cdr.getCellId());
            glEvent.setPlaceId(currentCell.getGeoLocationLevel2());
        } else {
            glEvent.setPlaceId(0);
        }

        outputValue.set(glEvent.build());
        context.write(this.outputKey, this.outputValue);
    }

    private void loadCellCatalogue(final Context context,
            final String hdfsFileLocation) {
        loadCellCatalogue(context.getConfiguration(), hdfsFileLocation);
    }

    private void loadCellCatalogue(final Configuration conf,
            final String hdfsFileLocation) {
        final FSDataInputStream in;
        final BufferedReader br;

        LOG.debug("Load Cell Catalogue from HDFS");

        this.cellsCataloge = CellCatalogue.getInstance();
        if (loadCatalogue) {
            return;
        }

        try {

            final FileSystem fs = FileSystem.get(conf);
            final Path path = new Path(hdfsFileLocation);

            in = fs.open(path);
            br = new BufferedReader(new InputStreamReader(in));
        } catch (FileNotFoundException fnfe) {
            LOG.error("Read from distributed cache: file not found");
            return;
        } catch (IOException ioe) {
            LOG.error("Read from distributed cache: IO exception");
            return;
        }

        try {

            final ParserCell cellParser = new ParserFactory().createNewCellParser(CELL_PARSER);

            String line;
            while ((line = br.readLine()) != null) {
                final Cell cell = cellParser.parseCellLine(line);
                this.cellsCataloge.addCell(cell);
            }

            loadCatalogue = true;

            in.close();
        } catch (IOException ioe) {
            LOG.debug("read from distributed cache: read length and instances");
        } catch (NullPointerException npe) {
            LOG.debug("read from distributed cache: read length and instances");
        }
    }
}
