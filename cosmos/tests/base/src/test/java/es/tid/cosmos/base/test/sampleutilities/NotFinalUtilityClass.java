package es.tid.cosmos.base.test.sampleutilities;

/**
 * Sample invalid utility class for testing UtilityClassTest.
 *
 * It is not final.
 *
 * @author sortega
 */
public class NotFinalUtilityClass {

    private NotFinalUtilityClass() {
        // Utility class
    }

    public static int sampleMethod(int in) {
        return 2 * in;
    }
}
