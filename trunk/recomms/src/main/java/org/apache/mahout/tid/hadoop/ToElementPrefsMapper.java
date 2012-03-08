package org.apache.mahout.tid.hadoop;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.mahout.cf.taste.hadoop.item.RecommenderJob;
import org.apache.mahout.cf.taste.hadoop.EntityPrefWritable;
import org.apache.mahout.math.VarLongWritable;

import java.io.IOException;

public class ToElementPrefsMapper
		extends
		Mapper<VarLongWritable, EventsWritable, VarLongWritable, VarLongWritable> {

	private boolean booleanData;

	public ToElementPrefsMapper() {
	}

	@Override
	protected void setup(Context context) {
		Configuration jobConf = context.getConfiguration();
		booleanData = jobConf.getBoolean(RecommenderJob.BOOLEAN_DATA, false);
	}

	@Override
	public void map(VarLongWritable key, EventsWritable value, Context context)
			throws IOException, InterruptedException {

		long userID = Long.parseLong(key.toString());
		long itemID = value.getID();

		if (booleanData) {
			context.write(new VarLongWritable(userID), new VarLongWritable(
					itemID));
		} else {
			context.write(new VarLongWritable(userID), new EntityPrefWritable(
					itemID, value.getEventValue()));
		}
	}

}
