package es.tid.bdp.profile.dictionary.comscore;

import java.io.File;
import java.io.IOException;
import java.net.URI;
import java.util.LinkedList;
import java.util.List;

import org.apache.hadoop.filecache.DistributedCache;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Reducer.Context;

/**
 *
 * @author dmicol, sortega
 */
public abstract class DistributedCacheDictionary {
    public static final String LATEST_DICTIONARY =
            "/user/hdfs/comscore/latest/";
    private static final String DICTIONARY_NAMES =
            "comscore.dict.filenames";
    private static final String[] DEFAULT_DICTIONARY_NAMES = new String[] {
            "cs_terms_in_domain.bcp",
            "cs_mmxi.bcp.gz",
            "patterns_to_categories.txt",
            "cat_subcat_map.txt"
    };
    private static final int TERMS_PATH = 0;
    private static final int DICTIONARY_PATH = 1;
    private static final int CATEGORY_PATTERN_MAPPING_PATH = 2;
    private static final int CATEGORY_NAMES_PATH = 3;

    /**
     * Configure the distributed cache to share the dictionary files to all
     * the workers.
     *
     * @param job              job to configure
     * @param baseDirectory    dfs path to the dictionary files
     * @param dictionaryNames  name of the files to cache
     */
    public static void cacheDictionary(Job job, String baseDirectory,
            String[] dictionaryNames) {
        DistributedCache.createSymlink(job.getConfiguration());
        if (!baseDirectory.endsWith(File.separator)) {
            baseDirectory += File.pathSeparator;
        }
        for (String name : dictionaryNames) {
            DistributedCache.addCacheFile(URI.create(baseDirectory + name),
                    job.getConfiguration());
        }
        job.getConfiguration().setStrings(DICTIONARY_NAMES, dictionaryNames);
    }

    /**
     * Configure the distributed cache to share the dictionary files to all
     * the workers.
     *
     * Dictionary files are assume to be named as in DEFAULT_DICTIONARY_NAMES
     *
     * @see DistributedCacheDictionary#DEFAULT_DICTIONARY_NAMES
     *
     * @param job              job to configure
     * @param baseDirectory    dfs path to the dictionary files
     */
    public static void cacheDictionary(Job job, String baseDirectory) {
        cacheDictionary(job, baseDirectory, DEFAULT_DICTIONARY_NAMES);
    }

    /**
     * Creates and initializes a dictionary from files allocated in the
     * distributed cache.
     *
     * Use in conjunction with #cacheDictionary
     *
     * @param context Hadoop context
     * @return        A dictionary
     * @throws IOException
     */
    public static CSDictionary loadFromCache(Context context)
            throws IOException {
        String[] dictionaryNames = context.getConfiguration()
                .getStrings(DICTIONARY_NAMES, DEFAULT_DICTIONARY_NAMES);
        List<String> cachedFiles = getCachedPaths(context, dictionaryNames);
        CSDictionary dictionary = new CSDictionary(
                cachedFiles.get(TERMS_PATH),
                cachedFiles.get(DICTIONARY_PATH),
                cachedFiles.get(CATEGORY_PATTERN_MAPPING_PATH),
                cachedFiles.get(CATEGORY_NAMES_PATH),
                CSDictionaryJNIInterface.DEFAULT_COMSCORE_LIBS);
        dictionary.init();
        return dictionary;
    }

    public static List<String> getCachedPaths(Context context,
            String[] fileNames) throws IOException {
        List<String> cachedPaths = new LinkedList<String>();
        for (String fileName : fileNames) {
            cachedPaths.add(getCachedPath(context, fileName));
        }
        return cachedPaths;
    }

    public static String getCachedPath(Context context, String fileName)
            throws IOException {
        for (Path path : DistributedCache.getLocalCacheFiles(
                context.getConfiguration())) {
            if (path.getName().equals(fileName)) {
                return path.toString();
            }
        }
        throw new IllegalArgumentException("'" + fileName
                + "' is not in the distributed cache");
    }
}
