package es.tid.cosmos.kpicalculation;

import java.io.IOException;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.mapreduce.CompositeKey;

/**
 * This class makes the reduce phase in the kpi aggregates grouped by field
 * calculation of the web profiling module. When the target aggregation is a
 * counter of unique values of a field of the items, depending on the values of
 * some other of its fields, this reducer will be used.
 *
 * The fields will be received sorted from the sorting phase depending on their
 * primary key ( filtering fields ), so in the reduce phase the number of
 * different elements ( depending on the grouping field ) with the same primary
 * key will be counted. When an item with a different primary key is received,
 * the previous key is emitted to the output and a new counting process for the
 * new key is performed.
 *
 * In order to emit the last key will be necessary to perform a write operation
 * in the cleanup phase.
 *
 * @author javierb
 */
public class KpiCounterByReducer extends
        Reducer<CompositeKey, IntWritable, Text, IntWritable> {
    private static final String USE_HASHCODE = "kpi.aggregation.hashmap";
    private String currentKey;
    private int currentHashCode;
    private int currentValue;
    private IntWritable counter;
    private Text text;
    // Defines if the hashCode is written as the first field of the output
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
        this.currentKey = "";
        this.currentHashCode = 0;
        this.currentValue = 0;
        this.counter = new IntWritable();
        this.text = new Text();
        this.useHashCode = context.getConfiguration().getBoolean(USE_HASHCODE,
                false);
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
    protected void reduce(CompositeKey key, Iterable<IntWritable> values,
            Context context) throws IOException, InterruptedException {
        final String primaryKey = key.get(0);
        if (!primaryKey.equals(this.currentKey)) {
            if (!this.currentKey.isEmpty()) {
                this.setValues();
                context.write(this.text, this.counter);
            }
            this.currentKey = primaryKey;
            this.currentHashCode = key.hashCode();
            this.currentValue = 1;
        } else {
            this.currentValue++;
        }
    }

    /**
     * The cleanup phase has to be override to be able to emit the last key
     * retrieved in the reduce process
     *
     * @param context
     *            contains the context of the job run
     */
    @Override
    protected void cleanup(Context context) throws IOException,
            InterruptedException {
        this.setValues();
        context.write(this.text, this.counter);
        super.cleanup(context);
    }

    /**
     * Method that prepares the values to be emit before writing them to the
     * output.
     */
    private void setValues() {
        if (this.useHashCode) {
            this.text.set(this.currentHashCode + "\t" + this.currentKey);
        } else {
            this.text.set(this.currentKey);
        }
        this.counter.set(this.currentValue);
    }
}