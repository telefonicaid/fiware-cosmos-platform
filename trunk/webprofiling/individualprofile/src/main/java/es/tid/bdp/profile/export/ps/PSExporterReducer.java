package es.tid.bdp.profile.export.ps;

import java.io.IOException;
import java.util.Iterator;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Counter;
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
    private StringBuilder builder;
    private Text record;

    @Override
    public void setup(Context context) throws IOException {
        this.builder = new StringBuilder();
        this.record = new Text();
    }
    
    @Override
    public void reduce(Text userId,
                       Iterable<ProtobufWritable<UserProfile>> profiles,
                       Context context) throws IOException,
                                               InterruptedException {
        Counter recordCount = context.getCounter(PSExporterCounter.NUM_RECORDS);
        for (Iterator<ProtobufWritable<UserProfile>> it = profiles.iterator();
                it.hasNext();) {
            final ProtobufWritable<UserProfile> wrappedProfile = it.next();
            wrappedProfile.setConverter(UserProfile.class);
            UserProfile profile = wrappedProfile.get();

            if (recordCount.getValue() == 0L) {
                this.builder.setLength(0);
                this.builder.append("User");
                this.builder.append("|");
                for (CategoryCount count : profile.getCountsList()) {
                    this.builder.append(count.getName());
                    this.builder.append("|");
                }
            }
            this.record.set(this.builder.toString());
            context.write(NullWritable.get(), this.record);

            this.builder.setLength(0);
            String userIdAndDate = userId + "_" + profile.getDate();
            this.builder.append(userIdAndDate);
            for (CategoryCount count : profile.getCountsList()) {
                this.builder.append(count.getCount());
                this.builder.append("|");
            }
            this.record.set(this.builder.toString());
            context.write(NullWritable.get(), this.record);

            recordCount.increment(1L);
        }
    }
}
