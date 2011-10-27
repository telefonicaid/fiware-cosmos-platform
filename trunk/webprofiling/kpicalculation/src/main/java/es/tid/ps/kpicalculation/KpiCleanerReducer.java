package es.tid.ps.kpicalculation;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

public class KpiCleanerReducer extends Reducer<LongWritable, Text, NullWritable, Text> {

    protected void reduce(LongWritable key, Iterable<Text> values, Context context) throws IOException,
            InterruptedException {
        // ArrayListWritable out = new ArrayListWritable();
        NullWritable k = null;
        String str = null;
        Text out = null;
        for (Text text : values) {
            if (str != text.toString()) {
                str = text.toString();
                out = text;
            }
        }
        context.write(k, out);
    }
}
