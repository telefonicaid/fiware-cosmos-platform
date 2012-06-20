package es.tid.cosmos.tests.hadoopjobs.parameterizedjob.base;

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
public class MultiplePrintReducer
        extends Reducer<Text, IntWritable, Text, BSONWritable> {
    @Override
    public void reduce(Text key, Iterable<IntWritable> values, Context context) 
            throws IOException, InterruptedException {
        if (MultiplePrintMapper.KEY_LIST.equals(key)) {
            List<Integer> valueList = new ArrayList<Integer>();
            for (IntWritable i : values) {
                valueList.add(i.get());
            }
            context.write(key, this.toBSON(valueList.toArray(new Integer[0])));
        } else {
            throw new IllegalStateException("Unexpected key: " + key.toString());
        }
    }
    
    private BSONWritable toBSON(Integer[] multiples) {
        BSONObject obj = new BasicBSONObject();
        obj.put("multiples", multiples);
        return new BSONWritable(obj);
    }
}
