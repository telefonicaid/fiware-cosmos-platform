package es.tid.analytics.mobility.core;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.analytics.mobility.core.data.Client;
import es.tid.analytics.mobility.core.data.GLEvent;

public class IndividualMobilityReduce extends
		Reducer<LongWritable, GLEvent, LongWritable, Client> {

	int cont = 0;

	@Override
	protected void reduce(final LongWritable key,
			final Iterable<GLEvent> values, final Context context)
			throws IOException, InterruptedException {
		// Initialize client object
		Client client = new Client();

		client.setIdNode(key.get());
		client.calculateGeoLocations(values.iterator());
		client.calculateNodeCommVector();
		client.calculatePoiCommVector();

		context.write(key, client);

	}
}