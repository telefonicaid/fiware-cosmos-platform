package es.tid.ps.dynamicprofile.dictionary;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.io.IOException;
import java.util.HashMap;

public abstract class DictionaryHandler {
    private final static String DELIMITER = "\t";
    
    private static CSDictionaryJNIInterface dictionary = null;
    private static HashMap<Long, Long> categoryPatternMap = null;
    private static HashMap<Long, String> categoryNames = null;
    private static boolean isInitialized = false;

    public static void init(String termsInDomainFlatFileName,
            String dictionaryFileName, String categoryPatterMappingFileName,
            String categoryNamesFileName) throws IOException {
        if (isInitialized) {
            return;
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
        long patternId = dictionary.ApplyDictionaryUsingUrl(url);
        if (patternId < 1) {
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
