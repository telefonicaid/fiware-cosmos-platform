package es.tid.ps.mapreduce.sna;

import java.io.IOException;

import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

/**
 * This class implements the reducer for generate the social graph. This class
 * receives the phone number as key, and all the phones with it has been a
 * conference. This reduce join all of this data into only one line. This
 * process delete the duplicates.
 * <ol>
 * <li>Input: 619555666 {619777888, 619777888, 619777888 }</li>
 * <li>output: { 619555666, 619777888, 619777888}</li>
 * </ol>
 * 
 * @author rgc@tid.es
 */
public class SocialGraphReducer extends
        Reducer<Text, Text, Text, ArrayListWritable> {

    /**
     * @param key
     *            is the key of the mapper
     * @param values
     *            are all the values aggregated during the mapping phase
     * @param context
     *            contains the context of the job run
     */
    protected void reduce(Text key, Iterable<Text> values, Context context)
            throws IOException, InterruptedException {
        ArrayListWritable out = new ArrayListWritable();
        String str = null;
        for (Text text : values) {
            if (str != text.toString()) {
                str = text.toString();
                out.add(str);
            }
        }
        context.write(key, out);
    }
}