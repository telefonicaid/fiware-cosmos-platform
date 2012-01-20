package es.tid.ps.kpicalculation;

import java.io.IOException;
import java.util.Iterator;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.ps.kpicalculation.data.WebLog;

/**
 * This class makes the reduce phase in the simple kpi aggregates calculation of
 * the web profiling module.When the target aggregation is a simple counter of
 * items depending on the values of some of its fields, this reducer will be
 * used.
 * 
 * For each key received, there will be a set of integer values corresponding to
 * the summatory of this key during the combining phase, so in the reduce method
 * a new summatory will be performed with these values, to emit the total result
 * as output.
 * 
 * @author javierb
 */
public class KpiCounterReducer extends
        Reducer<WebLog, IntWritable, Text, IntWritable> {
    private static final String USE_HASHCODE = "kpi.aggregation.hashmap";
    private IntWritable counter;
    private Text text;
    private boolean useHashCode;

    /**
     * Method that creates the objects that will be used during the reduce
     * 
     * @param context
     *            contains the context of the job run
     */
    @Override
    protected void setup(Context context) throws IOException,
            InterruptedException {
        this.useHashCode = context.getConfiguration().getBoolean(USE_HASHCODE,
                false);
        this.counter = new IntWritable();
        this.text = new Text();
    }

    /**
     * @param key
     *            is the key emitted by the mapper.
     * @param values
     *            are all the values aggregated during the mapping phase
     * @param context
     *            contains the context of the job run
     */
    @Override
    protected void reduce(WebLog key, Iterable<IntWritable> values,
            Context context) throws IOException, InterruptedException {
        int count = 0;

        Iterator<IntWritable> it = values.iterator();
        while (it.hasNext()) {
            count += it.next().get();
        }
        this.counter.set(count);
        if (this.useHashCode) {
            this.text.set(key.hashCode() + "\t" + key.mainKey);
        } else {
            this.text.set(key.mainKey);
        }
        context.write(this.text, this.counter);
    }
}
