package es.tid.ps.profile.categoryextraction;

import es.tid.ps.base.mapreduce.BinaryKey;
import es.tid.ps.profile.dictionary.Categorization;
import es.tid.ps.profile.dictionary.Dictionary;
import es.tid.ps.profile.dictionary.comscore.CSDictionary;
import org.apache.avro.mapred.Pair;
import org.apache.avro.mapreduce.AvroReducer;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.filecache.DistributedCache;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

/*
 * Enum with the list of counters to use in the CategoryExtraction mapreduces.
 *
 * @author dmicol, sortega
 */
public class CategoryExtractionReducer extends AvroReducer<BinaryKey,
        UserNavigation, Pair<BinaryKey, CategoryInformation>> {
    private static Dictionary dictionary = null;
    private CategoryInformation catInfo;

    @Override
    protected void setup(Context context) throws IOException, InterruptedException {
        this.setupDictionary(context.getConfiguration());
        this.catInfo = new CategoryInformation();
    }

    protected void setupDictionary(Configuration conf) throws IOException {
        if (dictionary == null) {
            dictionary = new CSDictionary(
                    DistributedCache.getLocalCacheFiles(conf));
            dictionary.init();
        }
    }

    @Override
    protected void doReduce(BinaryKey key, Iterable<UserNavigation> values, Context context)
            throws IOException, InterruptedException {
        Map<String, Long> uniqueUrlCounts = this.getUniqueUrlCounts(values);
        for (String url : uniqueUrlCounts.keySet()) {
            long urlInstances = uniqueUrlCounts.get(url);
            Categorization dictionaryResponse = this.categorize(url);

            switch (dictionaryResponse.getResult()) {
                case KNOWN_URL:
                    context.getCounter(CategoryExtractionCounter.KNOWN_VISITS).
                            increment(urlInstances);
                    CategoryInformationFactory.set(this.catInfo,
                            key.getPrimaryKey(), url, key.getSecondaryKey(), urlInstances, dictionaryResponse.getCategories());
                    write(context, new Pair<BinaryKey, CategoryInformation>(key, this.catInfo));
                    break;

                case IRRELEVANT_URL:
                    context.getCounter(
                            CategoryExtractionCounter.IRRELEVANT_VISITS).
                            increment(urlInstances);
                    break;

                case UNKNOWN_URL:
                    // TODO: do something smart for URL category extraction.
                    context.getCounter(
                            CategoryExtractionCounter.UNKNOWN_VISITS).
                            increment(urlInstances);
                    break;

                case GENERIC_FAILURE:
                    context.getCounter(
                            CategoryExtractionCounter.UNPROCESSED_VISITS).
                            increment(urlInstances);
                    break;

                default:
                    throw new IllegalStateException(
                            "Invalid dictionary response.");
            }
        }
    }

    protected Categorization categorize(String url) {
        return dictionary.categorize(url);
    }
    
    private Map<String, Long> getUniqueUrlCounts(
            Iterable<UserNavigation> values) {
        Map<String, Long> uniqueUrlCounts = new HashMap<String, Long>();
        for (UserNavigation nav : values) {
            Long count;
            if (uniqueUrlCounts.containsKey(nav.getFullUrl().toString())) {
                count = uniqueUrlCounts.get(nav.getFullUrl().toString());
            } else {
                count = 0L;
            }
            uniqueUrlCounts.put(nav.getFullUrl().toString(), count + 1L);
        }
        return uniqueUrlCounts;
    }
}
