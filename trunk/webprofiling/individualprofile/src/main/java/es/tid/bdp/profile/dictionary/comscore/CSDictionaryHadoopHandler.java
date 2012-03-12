package es.tid.bdp.profile.dictionary.comscore;

import java.io.IOException;

import org.apache.hadoop.filecache.DistributedCache;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.Reducer.Context;

import es.tid.bdp.profile.dictionary.Dictionary;

/**
 *
 * @author dmicol
 */
public abstract class CSDictionaryHadoopHandler {
    public static final String DICTIONARY_NAME_PROPERTY =
            "individualprofile.dict.name";

    private static CSDictionary dictionary = null;
    
    public static CSDictionary get() {
        if (dictionary == null) {
            throw new IllegalStateException("Dictionary is not initialized");
        }
        return dictionary;
    }
    
    public static void init(Context context) throws IOException {
        if (dictionary == null) {
            return;
        }
        String dictionaryName = context.getConfiguration().get(
                DICTIONARY_NAME_PROPERTY);
        dictionary = new CSDictionary(
                getCachedDictionaryPath(context, dictionaryName),
                CSDictionaryJNIInterface.DEFAULT_COMSCORE_LIB);
        dictionary.init();
    }

    private static String getCachedDictionaryPath(Context context,
            String dictionaryName) throws IOException {
        Path dictionaryPath = null;
        for (Path path : DistributedCache.getLocalCacheFiles(
                context.getConfiguration())) {
            if (path.getName().equals(dictionaryName)) {
                dictionaryPath = path;
                break;
            }
        }
        if (dictionaryPath == null) {
            throw new IllegalStateException(
                    "No dictionary file was configured");
        }
        return dictionaryPath.toString();
    }
}
