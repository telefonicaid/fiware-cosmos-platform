/**
 * Class that defines the native methods to access the comScore
 * dictionary API via JNI.
 * 
 * @author dmicol
 */
public class Categories {
    public native boolean InitFromTermsInDomainFlatFile(int iMode,
            String szTermsInDomainFlatFileName);

    public native boolean LoadCSDictionary(int iMode,
            String szTermsInDomainFlatFileName, String szDictionaryName);

    public native boolean ApplyDictionaryUsingUrl(String szURL);

    static {
        System.loadLibrary("Categories");
    }

    public static void main(String[] args) {
    }
}
