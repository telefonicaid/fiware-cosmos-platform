package es.tid.ps.profile.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.ps.profile.data.ProfileProtocol.CategoryCount;

/**
 * Construction utils for CategoryCount
 *
 * @author sortega
 */
public abstract class CategoryCountUtil {
    private CategoryCountUtil() {
    }
    
    public static CategoryCount create(String name, long count) {
        return ProfileProtocol.CategoryCount
                .newBuilder()
                .setName(name)
                .setCount(count)
                .build();
    }

    public static ProtobufWritable<CategoryCount> createAndWrap(String name,
            long count) {
        ProtobufWritable<CategoryCount> wrapper =
                ProtobufWritable.newInstance(CategoryCount.class);
        wrapper.set(create(name, count));
        return wrapper;
    }
}
