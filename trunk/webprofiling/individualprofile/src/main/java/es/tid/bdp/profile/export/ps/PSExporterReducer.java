package es.tid.bdp.profile.export.ps;

import java.io.IOException;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Counter;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.bdp.profile.data.ProfileProtocol.CategoryCount;
import es.tid.bdp.profile.data.ProfileProtocol.UserProfile;
import es.tid.bdp.profile.dictionary.Dictionary;
import es.tid.bdp.profile.dictionary.comscore.DistributedCacheDictionary;

/**
 *
 * @author dmicol, sortega
 */
public class PSExporterReducer extends Reducer<Text,
                                               ProtobufWritable<UserProfile>,
                                               NullWritable, Text> {
    private static Dictionary sharedDictionary = null;
    private static String[] sharedCategoryNames = null;

    private StringBuilder builder;
    private Text record;
    private Map<String, CategoryCount> categories;
    private Counter recordCounter;

    @Override
    public void setup(Context context) throws IOException {
        this.setupDictionary(context);

        this.builder = new StringBuilder();
        this.record = new Text();
        this.categories = new HashMap<String, CategoryCount>();
        this.recordCounter = context.getCounter(PSExporterCounter.NUM_RECORDS);
    }

    protected void setupDictionary(Context context) throws IOException {
        if (sharedDictionary == null) {
            sharedDictionary = DistributedCacheDictionary
                    .loadFromCache(context.getConfiguration());
            setCategoryNames(sharedDictionary.getAllCategoryNames());
        }
    }

    protected static void setCategoryNames(String[] categoryNames) {
        sharedCategoryNames = categoryNames;
    }

    @Override
    public void reduce(Text userId,
                       Iterable<ProtobufWritable<UserProfile>> profiles,
                       Context context) throws IOException,
                                               InterruptedException {
        for (Iterator<ProtobufWritable<UserProfile>> it = profiles.iterator();
                it.hasNext();) {
            final ProtobufWritable<UserProfile> wrappedProfile = it.next();
            wrappedProfile.setConverter(UserProfile.class);
            UserProfile profile = wrappedProfile.get();

            if (this.recordCounter.getValue() == 0L) {
                this.builder.setLength(0);
                this.builder.append("User");
                for (String categoryName : sharedCategoryNames) {
                    this.builder.append("|");
                    this.builder.append(categoryName);
                }
                this.record.set(this.builder.toString());
                context.write(NullWritable.get(), this.record);
            }

            this.categories.clear();
            for (CategoryCount count : profile.getCountsList()) {
                this.categories.put(count.getName(), count);
            }

            this.builder.setLength(0);
            String userIdAndDate = userId + "_" + profile.getDate();
            this.builder.append(userIdAndDate);
            for (String categoryName : sharedCategoryNames) {
                this.builder.append("|");
                long value = 0L;
                if (this.categories.containsKey(categoryName)) {
                    CategoryCount count =
                            (CategoryCount)this.categories.get(categoryName);
                    value = count.getCount();
                }
                this.builder.append(value);
            }
            this.record.set(this.builder.toString());
            context.write(NullWritable.get(), this.record);

            this.recordCounter.increment(1L);
        }
    }

    @Override
    public void cleanup(Context context) throws IOException,
                                                InterruptedException {
        this.builder.setLength(0);
        this.builder.append(this.recordCounter.getValue());
        this.record.set(this.builder.toString());
        context.write(NullWritable.get(), this.record);
    }
}
