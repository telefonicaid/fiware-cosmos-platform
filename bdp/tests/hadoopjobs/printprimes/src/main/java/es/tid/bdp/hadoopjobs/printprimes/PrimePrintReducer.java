package es.tid.bdp.hadoopjobs.printprimes;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.apache.hadoop.io.ArrayWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.io.Writable;
import org.apache.hadoop.mapreduce.Reducer;

/**
 *
 * @author ximo
 */
public class PrimePrintReducer
        extends Reducer<Text, IntWritable, Text, ArrayWritable> {
    @Override
    public void reduce(Text key, Iterable<IntWritable> values, Context context) 
            throws IOException, InterruptedException {
        // Ignore KEY_COUNT for now
        if (PrimePrintMapper.KEY_LIST.equals(key)) {
            List<IntWritable> valueList = new ArrayList<IntWritable>();
            for (IntWritable i : values) {
                valueList.add(i);            
            }
            ArrayWritable array = new ArrayWritable(
                    IntWritable.class,
                    valueList.toArray(new Writable[0]));
            context.write(key, array);
        }
    }
}
