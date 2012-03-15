package es.tid.bdp.kpicalculation.export.mongodb;

import java.io.IOException;

import com.mongodb.hadoop.io.BSONWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;
import org.bson.BSONObject;
import org.bson.BasicBSONObject;

/**
 *
 * @author dmicol, sortega
 */
public class MongoDBExporterReducer extends Reducer<Text, IntWritable,
        NullWritable, BSONWritable> {
    @Override
    public void reduce(Text key, Iterable<IntWritable> counts,
                       Context context) throws IOException,
                                               InterruptedException {
        String[] attributes = key.toString().split("\t");
        for (IntWritable count : counts) {
            context.write(NullWritable.get(), this.toBSON(attributes, count));
        }
    }

    private BSONWritable toBSON(String[] attributes, IntWritable count) {
        BSONObject obj = new BasicBSONObject();
        obj.put("attributes", attributes);
        obj.put("count", count);
        return new BSONWritable(obj);
    }
}
