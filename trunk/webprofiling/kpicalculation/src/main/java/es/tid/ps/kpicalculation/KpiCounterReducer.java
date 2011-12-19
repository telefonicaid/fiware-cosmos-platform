package es.tid.ps.kpicalculation;

import java.io.IOException;

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

    private IntWritable counter;
    private Text text;

    /**
     * Method that creates the objects that will be used during the reduce
     * 
     * @param context
     *            contains the context of the job run
     */
    @Override
    protected void setup(Context context) throws IOException,
            InterruptedException {
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

        int res = 0;

        while (values.iterator().hasNext()) {
            res += values.iterator().next().get();
        }
        counter.set(res);
        text.set(key.mainKey);
        context.write(text, counter);
    }
}
