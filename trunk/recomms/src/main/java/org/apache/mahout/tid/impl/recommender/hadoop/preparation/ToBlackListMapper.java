package org.apache.mahout.tid.impl.recommender.hadoop.preparation;

import java.io.IOException;

import org.apache.hadoop.mapreduce.Mapper;

import org.apache.mahout.math.VarLongWritable;
import org.apache.mahout.tid.hadoop.EventsWritable;

public final class ToBlackListMapper
        extends
        Mapper<VarLongWritable, EventsWritable, VarLongWritable, VarLongWritable> {
    @Override
    public void map(VarLongWritable key, EventsWritable value, Context context)
            throws IOException, InterruptedException {
        long itemID = Long.valueOf(value.getID());

        if (value.getEventType() == 0 || value.getEventType() == 1) {
            context.write(key, new VarLongWritable(itemID));
        }

    }
}
