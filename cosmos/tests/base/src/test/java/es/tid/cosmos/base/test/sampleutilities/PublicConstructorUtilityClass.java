package es.tid.cosmos.base.test.sampleutilities;

/**
 * Sample invalid utility class for testing UtilityClassTest.
 *
 * It has a public constructor.
 *
 * @author sortega
 */
public final class PublicConstructorUtilityClass {
    private final int i;

    public PublicConstructorUtilityClass(int i) {
        this.i = i;
    }

    public static int sampleMethod(int in) {
        return 2 * in;
    }
}
