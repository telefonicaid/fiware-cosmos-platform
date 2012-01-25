package es.tid.ps.profile.dictionary.comscore;

/**
 * Class that defines the native methods to access the comScore dictionary API
 * via JNI.
 *
 * @author dmicol
 */
public class CSDictionaryJNIInterface {
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
        System.load("/opt/hadoop/lib/native/Linux-amd64-64/libcsCFD.so.1");
        System.load("/opt/hadoop/lib/native/Linux-amd64-64/libCategories.so");
    }
}
