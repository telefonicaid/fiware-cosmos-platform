package es.tid.cosmos.profile.userprofile;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.mapreduce.BinaryKey;
import es.tid.cosmos.profile.generated.data.ProfileProtocol.CategoryCount;
import es.tid.cosmos.profile.generated.data.ProfileProtocol.CategoryInformation;

/**
 * Maps <[userId, date], categoryInfo> to <[userId, date], [category, count]>
 *
 * @author sortega@tid.es
 */
public class UserProfileMapper extends Mapper<BinaryKey,
        ProtobufWritable<CategoryInformation>,
        BinaryKey, ProtobufWritable<CategoryCount>> {
    private ProtobufWritable<CategoryCount> categoryCountWrapper;

    @Override
    protected void setup(Context context) throws IOException,
            InterruptedException {
        this.categoryCountWrapper = new ProtobufWritable<CategoryCount>();
        this.categoryCountWrapper.setConverter(CategoryCount.class);
    }

    @Override
    protected void map(BinaryKey userDateKey,
            ProtobufWritable<CategoryInformation> categoryInfoValue,
            Context context) throws IOException, InterruptedException {
        categoryInfoValue.setConverter(CategoryInformation.class);
        CategoryInformation categoryInfo = categoryInfoValue.get();
        for (String category : categoryInfo.getCategoriesList()) {
            this.categoryCountWrapper.set(CategoryCount
                    .newBuilder()
                    .setName(category)
                    .setCount(categoryInfo.getCount())
                    .build());
            context.write(userDateKey, this.categoryCountWrapper);
        }
    }
}
