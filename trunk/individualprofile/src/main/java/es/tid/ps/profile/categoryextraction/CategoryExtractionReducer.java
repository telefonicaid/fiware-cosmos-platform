package es.tid.ps.profile.categoryextraction;

import java.io.IOException;
import java.util.Iterator;
import org.apache.hadoop.filecache.DistributedCache;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.ps.profile.dictionary.Categorization;
import es.tid.ps.profile.dictionary.Dictionary;
import es.tid.ps.profile.dictionary.comscore.CSDictionary;

/*
 * Enum with the list of counters to use in the CategoryExtraction mapreduces.
 *
 * @author dmicol, sortega
 */
public class CategoryExtractionReducer extends Reducer<CompositeKey,
        NullWritable, CompositeKey, CategoryInformation> {
    private static Dictionary dictionary = null;
    private CategoryInformation catInfo;

    @Override
    public void setup(Context context) throws IOException {
        this.setupDictionary(context);
        this.catInfo = new CategoryInformation();
    }

    protected void setupDictionary(Context context) throws IOException {
        if (dictionary == null) {
            dictionary = new CSDictionary(
                    DistributedCache.getLocalCacheFiles(
                    context.getConfiguration()));
            dictionary.init();
        }
    }

    @Override
    protected void reduce(CompositeKey key, Iterable<NullWritable> values,
            Context context) throws IOException, InterruptedException {
        String url = key.getSecondaryKey();
        Categorization dictionaryResponse = this.categorize(url);
        long urlInstances = this.countURLInstances(values);

        switch (dictionaryResponse.result) {
            case KNOWN_URL:
                context.getCounter(CategoryExtractionCounter.KNOWN_VISITS).
                        increment(urlInstances);
                this.catInfo.setUserId(key.getPrimaryKey());
                this.catInfo.setUrl(key.getSecondaryKey());
                this.catInfo.setCount(urlInstances);
                this.catInfo.setCategoryNames(dictionaryResponse.categories);
                context.write(key, this.catInfo);
                return;

            case IRRELEVANT_URL:
                context.getCounter(CategoryExtractionCounter.IRRELEVANT_VISITS).
                        increment(urlInstances);
                return;

            case UNKNOWN_URL:
                context.getCounter(CategoryExtractionCounter.UNKNOWN_VISITS).
                        increment(urlInstances);
                // TODO: do something smart for URL category extraction.
                return;

            case GENERIC_FAILURE:
                context.getCounter(
                        CategoryExtractionCounter.UNPROCESSED_VISITS).
                        increment(urlInstances);
                return;

            default:
                throw new IllegalStateException("Invalid dictionary response.");
        }
    }

    protected Categorization categorize(String url) {
        return dictionary.categorize(url);
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
