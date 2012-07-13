package es.tid.cosmos.profile.export.text;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.io.NullWritable;

import es.tid.cosmos.profile.data.UserProfileUtil;
import es.tid.cosmos.profile.generated.data.ProfileProtocol.UserProfile;

/**
 *
 * @author dmicol
 */
public class TextExporterReducer extends Reducer<Text,
        ProtobufWritable<UserProfile>, NullWritable, Text> {
    @Override
    public void reduce(Text key,
            Iterable<ProtobufWritable<UserProfile>> values, Context context)
            throws IOException, InterruptedException {
        for (ProtobufWritable<UserProfile> value : values) {
            value.setConverter(UserProfile.class);
            context.write(NullWritable.get(), 
                          new Text(UserProfileUtil.toString(value.get())));
        }
    }
}
