package es.tid.cosmos.mobility.util;

import java.io.IOException;

import com.google.protobuf.Message;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.InputIdRecord;

/**
 *
 * @author dmicol
 */
public class SetMobDataInputIdReducer extends Reducer<LongWritable,
        MobilityWritable<Message>, LongWritable, MobilityWritable<InputIdRecord>> {
    private static final int DEFAULT_INVALID_ID = -1;
    private static Integer inputId = null;

    @Override
    protected void setup(Context context) throws IOException,
            InterruptedException {
        final Configuration conf = context.getConfiguration();
        inputId = conf.getInt("input_id", DEFAULT_INVALID_ID);
        if (inputId == DEFAULT_INVALID_ID) {
            throw new IllegalArgumentException("Missing or invalid input ID");
        }
    }
    
    @Override
    protected void reduce(LongWritable key,
            Iterable<MobilityWritable<Message>> values, Context context)
            throws IOException, InterruptedException {
        for (MobilityWritable<Message> value : values) {
            Message message = value.get();
            InputIdRecord record = InputIdRecord
                    .newBuilder()
                    .setInputId(inputId)
                    .setMessageBytes(message.toByteString())
                    .build();
            context.write(key, new MobilityWritable<InputIdRecord>(record));
        }
    }
}
