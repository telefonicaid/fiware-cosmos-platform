package es.tid.ps.profile.categoryextraction;

import java.io.IOException;
import java.util.Iterator;

import org.apache.hadoop.filecache.DistributedCache;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.ps.profile.dictionary.DictionaryHandler;

/*
 * Enum with the list of counters to use in the CategoryExtraction mapreduces.
 *
 * @author dmicol, sortega
 */
public class CategoryExtractionReducer extends
        Reducer<CompositeKey, NullWritable, CompositeKey, CategoryInformation> {
    private final static String CATEGORY_DELIMITER = "/";

    private CategoryInformation catInfo;

    @Override
    public void setup(Context context) throws IOException {
        this.setupDictionary(context);
        this.catInfo = new CategoryInformation();
    }

    protected void setupDictionary(Context context) throws IOException {
        DictionaryHandler.init(DistributedCache.getLocalCacheFiles(context
                .getConfiguration()));
    }

    @Override
    protected void reduce(CompositeKey key, Iterable<NullWritable> values,
            Context context) throws IOException, InterruptedException {
        String url = key.getSecondaryKey();
        String categories = this.getCategories(url);
        long urlInstances = this.countURLInstances(values);

        if (categories == null || categories.isEmpty()) {
            context.getCounter(CategoryExtractionCounter.UNKNOWN_URLS).
                    increment(1L);
            context.getCounter(CategoryExtractionCounter.UNKNOWN_VISITS).
                    increment(urlInstances);
            // TODO: do something smart for URL category extraction.
            return;
        }

        context.getCounter(CategoryExtractionCounter.KNOWN_URLS).
                increment(1L);
        context.getCounter(CategoryExtractionCounter.KNOWN_VISITS).
                increment(urlInstances);
        this.catInfo.setUserId(key.getPrimaryKey());
        this.catInfo.setUrl(key.getSecondaryKey());
        this.catInfo.setCount(urlInstances);
        this.catInfo.setCategoryNames(categories.split(CATEGORY_DELIMITER));
        context.write(key, this.catInfo);
    }

    protected String getCategories(String url) {
        // Use the comScore API.
        return DictionaryHandler.getUrlCategories(url);
    }

    private long countURLInstances(Iterable<NullWritable> values) {
        long count = 0l;
        Iterator<NullWritable> it = values.iterator();
        while (it.hasNext()) {
            count++;
            it.next();
        }
        return count;
    }
}
