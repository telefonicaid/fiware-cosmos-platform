package es.tid.ps.dynamicprofile.dictionary;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.io.IOException;
import java.net.URI;
import java.util.HashMap;
import org.apache.hadoop.fs.Path;

public abstract class DictionaryHandler {
    private final static String DELIMITER = "\t";

    private static CSDictionaryJNIInterface dictionary = null;
    private static HashMap<Long, Long> categoryPatternMap = null;
    private static HashMap<Long, String> categoryNames = null;
    private static boolean isInitialized = false;

    private static final String TERMS_IN_DOMAIN_FILENAME =
            "cs_terms_in_domain.bcp";
    private static final String DICTIONARY_FILENAME =
            "cs_mmxi_143.bcp.gz";
    private static final String CATEGORY_PATTERN_MAPPING_FILENAME =
            "pattern_category_mapping_143m.txt";
    private static final String CATEGORY_NAMES_FILENAME =
            "cat_subcat_lookup_143m.txt";

    public static void init(Path[] dictionayFiles) throws IOException {
        if (isInitialized) {
            return;
        }

        String termsInDomainFlatFileName = null;
        String dictionaryFileName = null;
        String categoryPatterMappingFileName = null;
        String categoryNamesFileName = null;

        for (Path path: dictionayFiles) {
            if (path.getName().equals(TERMS_IN_DOMAIN_FILENAME)) {
                termsInDomainFlatFileName = path.toString();
            } else if (path.getName().equals(DICTIONARY_FILENAME)) {
                dictionaryFileName = path.toString();
            } else if (path.getName().equals(CATEGORY_PATTERN_MAPPING_FILENAME)) {
                categoryPatterMappingFileName = path.toString();
            } else if (path.getName().equals(CATEGORY_NAMES_FILENAME)) {
                categoryNamesFileName = path.toString();
            }
        }

        dictionary = new CSDictionaryJNIInterface();
        dictionary.LoadCSDictionary(1, termsInDomainFlatFileName,
                dictionaryFileName);
        loadCategoryPatternMapping(categoryPatterMappingFileName);
        loadCategoryNames(categoryNamesFileName);
        isInitialized = true;
    }

    public static String getUrlCategories(String url) {
        if (!isInitialized) {
            return null;
        }
        URI uri = URI.create(url);
        String normalizedUrl = uri.getHost() + uri.getPath();
        long patternId = dictionary.ApplyDictionaryUsingUrl(normalizedUrl);
        if (patternId <= 1) {
            return null;
        }
        if (!categoryPatternMap.containsKey(patternId)) {
            return null;
        }
        long categoryId = categoryPatternMap.get(patternId);
        if (!categoryNames.containsKey(categoryId)) {
            return null;
        }
        String categoryName = categoryNames.get(categoryId);
        return categoryName;
    }

    /**
     * Loads the file which contains the category pattern mappings.
     *
     * @param fileName
     *            the file name that contains the mappings.
     * @return the map of pattern IDs to category IDs.
     */
    private static void loadCategoryPatternMapping(String fileName)
            throws IOException {
        categoryPatternMap = new HashMap<Long, Long>();
        FileInputStream fis = new FileInputStream(fileName);
        BufferedReader br = new BufferedReader(new InputStreamReader(fis));
        while (br.ready()) {
            String line = br.readLine();
            String[] columns = line.trim().split(DELIMITER);
            Long patternID = Long.parseLong(columns[0]);
            Long categoryID = Long.parseLong(columns[columns.length - 1]);
            categoryPatternMap.put(patternID, categoryID);
        }
        br.close();
        fis.close();
    }

    /**
     * Loads the file that contains the category IDs and their corresponding
     * names.
     *
     * @param fileName
     *            the file name that contains the list of category IDs and
     *            names.
     * @return the map of category IDs to names.
     */
    private static void loadCategoryNames(String fileName) throws IOException {
        categoryNames = new HashMap<Long, String>();
        FileInputStream fis = new FileInputStream(fileName);
        BufferedReader br = new BufferedReader(new InputStreamReader(fis));
        while (br.ready()) {
            String line = br.readLine();
            String[] columns = line.trim().split(DELIMITER);
            Long categoryID = Long.parseLong(columns[0]);
            String categoryName = columns[1];
            categoryNames.put(categoryID, categoryName);
        }
        br.close();
        fis.close();
    }
}
