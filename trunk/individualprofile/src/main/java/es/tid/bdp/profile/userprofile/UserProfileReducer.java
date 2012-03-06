package es.tid.bdp.profile.userprofile;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.ps.base.mapreduce.BinaryKey;
import es.tid.bdp.profile.data.ProfileProtocol.CategoryCount;
import es.tid.bdp.profile.data.ProfileProtocol.UserProfile;

/**
 * Aggregates category counts into a user profile.
 * <[userId, date], [category, count]> -> <userId, profile>
 *
 * @author sortega@tid.es
 */
public class UserProfileReducer extends Reducer<BinaryKey,
                                                ProtobufWritable<CategoryCount>,
                                                Text,
                                                ProtobufWritable<UserProfile>> {
    private UserProfile.Builder profile;
    private ProtobufWritable<UserProfile> profileWrapper;
    private Text userId;
    private CategoryCountAggregator categoryCountAggregator;

    @Override
    protected void setup(Context context) throws IOException,
            InterruptedException {
        this.profile = UserProfile.newBuilder();
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
        this.profile.clear();
        this.profile.setUserId(userDateKey.getPrimaryKey());
        this.profile.setDate(userDateKey.getSecondaryKey());

        this.categoryCountAggregator.clear();
        for (ProtobufWritable<CategoryCount> wrappedCount : counts) {
            wrappedCount.setConverter(CategoryCount.class);
            CategoryCount count = wrappedCount.get();
            categoryCountAggregator.add(count);
        }
        this.profile.addAllCounts(categoryCountAggregator.getSortedCounts());
        this.userId.set(userDateKey.getPrimaryKey());
        this.profileWrapper.set(this.profile.build());
        context.write(this.userId, this.profileWrapper);
    }
}
