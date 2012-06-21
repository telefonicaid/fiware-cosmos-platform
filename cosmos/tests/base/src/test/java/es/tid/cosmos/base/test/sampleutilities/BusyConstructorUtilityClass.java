package es.tid.cosmos.base.test.sampleutilities;

/**
 * Sample invalid utility class for testing UtilityClassTest
 *
 * Does significant work in the constructor.
 *
 * @author sortega
 */
public final class BusyConstructorUtilityClass {

    private BusyConstructorUtilityClass() throws Exception {
        throw new Exception("something that can throw exceptions");
    }

    public static BusyConstructorUtilityClass sampleMethod(int in)
            throws Exception {
        return new BusyConstructorUtilityClass();
    }
}
