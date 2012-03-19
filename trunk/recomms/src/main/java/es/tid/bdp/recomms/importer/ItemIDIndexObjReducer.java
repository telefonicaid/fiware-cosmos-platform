package es.tid.bdp.recomms.importer;

import java.io.IOException;

import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.IntWritable;

public final class ItemIDIndexObjReducer extends
        Reducer<IntWritable, LongWritable, IntWritable, LongWritable> {
    @Override
    public void reduce(IntWritable index,
            Iterable<LongWritable> possibleItemIDs, Context context)
            throws IOException, InterruptedException {
        long minimumItemID = Long.MAX_VALUE;
        for (LongWritable varLongWritable : possibleItemIDs) {
            long itemID = varLongWritable.get();
            if (itemID < minimumItemID) {
                minimumItemID = itemID;
            }
        }
        if (minimumItemID != Long.MAX_VALUE) {
            context.write(index, new LongWritable(minimumItemID));
        }
    }

}