package es.tid.cosmos.profile.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.profile.generated.data.ProfileProtocol;
import es.tid.cosmos.profile.generated.data.ProfileProtocol.CategoryCount;

/**
 * Utils for CategoryCount
 *
 * @author sortega
 */
public final class CategoryCountUtil {
    private static final String DELIMITER = "\t";
    
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
    
    public static String toString(CategoryCount obj) {
        return (obj.getName() + DELIMITER + obj.getCount());
    }
}
