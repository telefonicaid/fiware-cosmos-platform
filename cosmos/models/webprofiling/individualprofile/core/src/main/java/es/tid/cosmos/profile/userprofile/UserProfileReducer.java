package es.tid.cosmos.profile.userprofile;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.mapreduce.BinaryKey;
import es.tid.cosmos.profile.generated.data.ProfileProtocol.CategoryCount;
import es.tid.cosmos.profile.generated.data.ProfileProtocol.UserProfile;

/**
 * Aggregates category counts into a user profile.
 * <[userId, date], [category, count]> -> <userId, profile>
 *
 * @author sortega
 */
public class UserProfileReducer extends Reducer<BinaryKey,
        ProtobufWritable<CategoryCount>, Text, ProtobufWritable<UserProfile>> {
    private ProtobufWritable<UserProfile> profileWrapper;
    private Text userId;
    private CategoryCountAggregator categoryCountAggregator;

    @Override
    protected void setup(Context context) throws IOException,
            InterruptedException {
        this.profileWrapper = new ProtobufWritable<UserProfile>();
        this.profileWrapper.setConverter(UserProfile.class);
        this.userId = new Text();
        this.categoryCountAggregator = new CategoryCountAggregator();

    }

    @Override
    protected void reduce(BinaryKey userDateKey,
                          Iterable<ProtobufWritable<CategoryCount>> counts,
                          Context context) throws IOException,
                                                  InterruptedException {
        UserProfile.Builder profile = UserProfile.newBuilder()
                                  .setUserId(userDateKey.getPrimaryKey())
                                  .setDate(userDateKey.getSecondaryKey());

        this.categoryCountAggregator.clear();
        for (ProtobufWritable<CategoryCount> wrappedCount : counts) {
            wrappedCount.setConverter(CategoryCount.class);
            CategoryCount count = wrappedCount.get();
            this.categoryCountAggregator.add(count);
        }
        profile.addAllCounts(this.categoryCountAggregator.getSortedCounts());
        this.userId.set(userDateKey.getPrimaryKey());
        this.profileWrapper.set(profile.build());
        context.write(this.userId, this.profileWrapper);
    }
}
