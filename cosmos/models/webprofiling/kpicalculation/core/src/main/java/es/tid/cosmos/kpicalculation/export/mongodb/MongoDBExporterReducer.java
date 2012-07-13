package es.tid.cosmos.kpicalculation.export.mongodb;

import java.io.IOException;
import java.util.Arrays;
import java.util.ArrayList;
import java.util.List;

import com.mongodb.hadoop.io.BSONWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;
import org.bson.BSONObject;
import org.bson.BasicBSONObject;

/**
 *
 * @author dmicol, sortega
 */
public class MongoDBExporterReducer extends Reducer<LongWritable, Text,
        LongWritable, BSONWritable> {
    private static final String DELIMITER = "\t";
    
    private String name;
    private String[] fields;
    private List<String> columns;
    
    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        this.name = context.getConfiguration().get("name");
        this.fields = context.getConfiguration().getStrings("fields");
        this.columns = new ArrayList<String>();
    }

    @Override
    public void reduce(LongWritable key, Iterable<Text> values, Context context)
            throws IOException, InterruptedException {
        for (Text value : values) {
            this.columns.clear();
            this.columns.addAll(Arrays.asList(
                    value.toString().split(DELIMITER)));
            List<String> attributes = this.columns.subList(0,
                    this.columns.size() - 1);
            long count = Long.parseLong(this.columns.get(
                    this.columns.size() - 1));
            context.write(key, this.toBSON(attributes, count));
        }
    }

    private BSONWritable toBSON(List<String> attributes, long count) {
        if (this.fields.length != attributes.size()) {
            throw new IllegalStateException("Invalid aggregate data");
        }
        BSONObject obj = new BasicBSONObject();
        obj.put("name", this.name);
        for (int i = 0; i < attributes.size(); i++) {
            obj.put(this.fields[i], attributes.get(i));
        }
        obj.put("count", count);
        return new BSONWritable(obj);
    }
}
