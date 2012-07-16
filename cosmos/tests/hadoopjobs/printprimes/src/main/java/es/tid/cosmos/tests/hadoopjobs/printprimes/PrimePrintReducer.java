package es.tid.cosmos.tests.hadoopjobs.printprimes;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import com.mongodb.hadoop.io.BSONWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;
import org.bson.BSONObject;
import org.bson.BasicBSONObject;

/**
 *
 * @author ximo
 */
public class PrimePrintReducer
        extends Reducer<Text, IntWritable, Text, BSONWritable> {
    @Override
    public void reduce(Text key, Iterable<IntWritable> values, Context context)
            throws IOException, InterruptedException {
        // Ignore KEY_COUNT for now
        if (PrimePrintMapper.KEY_LIST.equals(key)) {
            List<Integer> valueList = new ArrayList<Integer>();
            for (IntWritable i : values) {
                valueList.add(i.get());
            }
            context.write(key, this.toBSON(valueList.toArray(new Integer[0])));
        }
    }

    private BSONWritable toBSON(Integer[] primes) {
        BSONObject obj = new BasicBSONObject();
        obj.put("primes", primes);
        return new BSONWritable(obj);
    }
}
