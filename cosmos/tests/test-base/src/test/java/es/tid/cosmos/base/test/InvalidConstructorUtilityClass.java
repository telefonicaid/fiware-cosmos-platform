package es.tid.cosmos.base.test;

/**
 * Sample invalid utility class for testing UtilityClassTest
 *
 * Constructor should have no arguments.
 *
 * @author sortega
 */
public final class InvalidConstructorUtilityClass {
    private static int i;

    private InvalidConstructorUtilityClass(int i) {
        InvalidConstructorUtilityClass.i = i;
    }

    public static int sampleMethod(int in) {
        return 2 * in;
    }
}
