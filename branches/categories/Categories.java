import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.io.IOException;
import java.util.HashMap;

/**
 * Class that defines the native methods to access the comScore dictionary API
 * via JNI.
 * 
 * @author dmicol
 */
public class Categories {
    private final static String DELIMITER = "\t";

    /**
     * Initializes the dictionary wrapper using the terms in domain file.
     * 
     * @param iMode
     *            the operation mode (value should be 1)
     * @param szTermsInDomainFlatFileName
     *            the terms in domain file name
     * @return whether the initialization succeeded
     */
    public native boolean InitFromTermsInDomainFlatFile(int iMode,
            String szTermsInDomainFlatFileName);

    /**
     * Loads the comScore dictionary in memory.
     * 
     * @param iMode
     *            the operation mode (value should be 1)
     * @param szTermsInDomainFlatFileName
     *            the terms in domain file name
     * @param szDictionaryName
     *            the file name of the dictionary
     * @return whether the load of the dictionary succeeded
     */
    public native boolean LoadCSDictionary(int iMode,
            String szTermsInDomainFlatFileName, String szDictionaryName);

    /**
     * Applies the dictionary to the given URL, and returns the pattern ID for
     * such URL.
     * 
     * @param szURL
     *            the url to apply the dictionary to
     * @return the pattern ID of the URL
     */
    public native long ApplyDictionaryUsingUrl(String szURL);

    static {
        System.loadLibrary("Categories");
    }

    /**
     * Loads the file which contains the category pattern mappings.
     * 
     * @param fileName
     *            the file name that contains the mappings.
     * @return the map of pattern IDs to category IDs.
     */
    public HashMap<Long, Long> loadCategoryPatternMapping(String fileName)
            throws IOException {
        HashMap<Long, Long> categoryPatternMap = new HashMap<Long, Long>();
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
        return categoryPatternMap;
    }

    /**
     * Loads the file that contains the category IDs and their corresponding names.
     * 
     * @param fileName
     *            the file name that contains the list of category IDs and names.
     * @return the map of category IDs to names.
     */
    public HashMap<Long, String> loadCategoryNames(String fileName)
            throws IOException {
        HashMap<Long, String> categoryNames = new HashMap<Long, String>();
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
        return categoryNames;
    }
}
