package es.tid.analytics.mobility.core;

import es.tid.analytics.mobility.core.data.Cdr;
import es.tid.analytics.mobility.core.data.Cell;
import es.tid.analytics.mobility.core.data.CellCatalogue;
import es.tid.analytics.mobility.core.data.GLEvent;
import es.tid.analytics.mobility.core.parsers.ParserCdr;
import es.tid.analytics.mobility.core.parsers.ParserCell;
import es.tid.analytics.mobility.core.parsers.ParserFactory;

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

public class IndividualMobilityMap extends
		Mapper<LongWritable, Text, LongWritable, GLEvent> {

	private static final Logger LOG = Logger.getLogger(IndividualMobilityMap.class);

	public static final String HDFS_CELL_CATALOGUE_PATH = "/data/cell.dat";
	private static final String CELL_PARSER = "DEFAULT";
	private static final String CDRS_PARSER = "DEFAULT";

	private CellCatalogue cellsCataloge;
	private static boolean loadCatalogue;
	private LongWritable outputKey = new LongWritable();
	private GLEvent event = new GLEvent();

	public IndividualMobilityMap() {
	}

	@Override
	protected void setup(final Context context) throws IOException,
			InterruptedException {
		loadCellCatalogue(context, HDFS_CELL_CATALOGUE_PATH);
	}

	@Override
	protected void map(final LongWritable key, final Text value,
			final Context context) throws IOException, InterruptedException {
		final ParserCdr cdrParser = new ParserFactory()
				.createNewCdrParser(CDRS_PARSER);
		final Cdr cdr = cdrParser.parseCdrsLine(value.toString());

		this.outputKey.set(cdr.getNode());
		this.event.setUserId(cdr.getNode());
		this.event.setDate(cdr.getDate());

		if (this.cellsCataloge != null
				&& this.cellsCataloge.containsCell(cdr.getIdCell())) {
			final Cell currentCell = this.cellsCataloge
					.getCell(cdr.getIdCell());
			this.event.setPlaceId(currentCell.getGeoLocationLevel2());
		} else {
			this.event.setPlaceId(0);
		}

		context.write(this.outputKey, this.event);
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
			fnfe.printStackTrace();
			LOG.error("Read from distributed cache: file not found");
			return;
		} catch (IOException ioe) {
			ioe.printStackTrace();
			LOG.error("Read from distributed cache: IO exception");
			return;
		}

		try {

			final ParserCell cellParser = new ParserFactory()
					.createNewCellParser(CELL_PARSER);

			String line;
			while ((line = br.readLine()) != null) {
				final Cell cell = cellParser.parseCellLine(line);
				this.cellsCataloge.addCell(cell);
			}

			loadCatalogue = true;

			in.close();
		} catch (IOException ioe) {
			ioe.printStackTrace();
			LOG.debug("read from distributed cache: read length and instances");
		} catch (NullPointerException npe) {
			npe.printStackTrace();
			LOG.debug("read from distributed cache: read length and instances");
		}
	}
}
