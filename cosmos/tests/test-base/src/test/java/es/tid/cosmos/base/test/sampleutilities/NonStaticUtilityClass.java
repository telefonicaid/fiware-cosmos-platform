package es.tid.cosmos.base.test.sampleutilities;

/**
 * Sample invalid utility class for testing UtilityClassTest.
 *
 * It has non-static methods.
 *
 * @author sortega
 */
public final class NonStaticUtilityClass {

    private NonStaticUtilityClass() {
        // Utility class
    }

    public int sampleMethod(int in) {
        return 2 * in;
    }
}
