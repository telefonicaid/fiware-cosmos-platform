package es.tid.bdp.profile.dictionary.comscore;

/**
 * Class that defines the native methods to access the comScore dictionary API
 * via JNI.
 *
 * @author dmicol, sortega
 */
public class CSDictionaryJNIInterface {
    public static final String COMSCORE_LIB_PROPERTY = "es.tid.comscore-path";

    /**
     * Loads the comScore dictionary in memory.
     *
     * @param dictionaryFilename
     *            the file name of the dictionary
     * @return whether the load of the dictionary succeeded
     */
    public native boolean loadCSDictionary(String dictionaryFilename);

    /**
     * Find outs the categories associated to an url.
     *
     * @param url
     *          the url to apply the dictionary to
     * @return the list of category IDs
     */
    public native int[] lookupCategories(String url);

    /**
     * Provides the name of a category.
     *
     * @param categoryId
     *            category ID provided by #lookupCategories
     * @return the category name
     */
    public native String getCategoryName(int categoryId);

    static {
        String comscorePath = System.getProperty(COMSCORE_LIB_PROPERTY);
        if (comscorePath != null && !comscorePath.isEmpty()) {
            System.load(comscorePath);
        } else {
            System.loadLibrary("comscore");
        }
    }
}
