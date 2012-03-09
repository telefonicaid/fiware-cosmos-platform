package es.tid.bdp.profile.export.ps;

import java.io.IOException;
import java.util.Iterator;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.bdp.profile.data.ProfileProtocol.CategoryCount;
import es.tid.bdp.profile.data.ProfileProtocol.UserProfile;

/**
 *
 * @author dmicol, sortega
 */
public class PSExporterReducer extends Reducer<Text,
                                               ProtobufWritable<UserProfile>,
                                               NullWritable, Text> {
    private static boolean headerAdded = false;
    
    @Override
    public void reduce(Text userId,
                       Iterable<ProtobufWritable<UserProfile>> profiles,
                       Context context) throws IOException,
                                               InterruptedException {
        for (Iterator<ProtobufWritable<UserProfile>> it = profiles.iterator();
                it.hasNext();) {
            if (!headerAdded) {
                // TODO: add header
                headerAdded = true;
            }

            final ProtobufWritable<UserProfile> wrappedProfile = it.next();
            wrappedProfile.setConverter(UserProfile.class);
            UserProfile profile = wrappedProfile.get();

            StringBuilder builder = new StringBuilder();
            String userIdAndDate = userId + "_" + profile.getDate();
            builder.append(userIdAndDate);
            for (CategoryCount count : profile.getCountsList()) {
                builder.append(count.getCount());
                builder.append("|");
            }
            context.write(NullWritable.get(), new Text(builder.toString()));
        }
    }
}
