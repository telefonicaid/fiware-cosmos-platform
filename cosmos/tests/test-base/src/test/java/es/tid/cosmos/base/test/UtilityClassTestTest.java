package es.tid.cosmos.base.test;

import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.ExpectedException;

import static es.tid.cosmos.base.test.UtilityClassTest.assertUtilityClass;

/**
 * @author sortega
 */
public class UtilityClassTestTest {
    @Rule
    public ExpectedException exception = ExpectedException.none();

    @Test
    public void testValidUtilityClass() {
        assertUtilityClass(ValidUtilityClass.class);
        assertUtilityClass(UtilityClassTest.class);
    }

    @Test
    public void testNotFinalUtilityClass() {
        this.exception.expect(AssertionError.class);
        this.exception.expectMessage("Utility class should be final");
        assertUtilityClass(NotFinalUtilityClass.class);
    }

    @Test
    public void testPublicConstructorUtilityClass() {
        this.exception.expect(AssertionError.class);
        this.exception.expectMessage("Utility class should not have " +
                                     "public constructors");
        assertUtilityClass(PublicConstructorUtilityClass.class);
    }

    @Test
    public void testNonStaticUtilityClass() {
        this.exception.expect(AssertionError.class);
        this.exception.expectMessage("Utility class should have static " +
                                     "methods only");
        assertUtilityClass(NonStaticUtilityClass.class);
    }

    @Test
    public void testInvalidConstructorUtilityClass() {
        this.exception.expect(AssertionError.class);
        this.exception.expectMessage("Utility class constructor should have " +
                                     "no arguments");
        assertUtilityClass(InvalidConstructorUtilityClass.class);
    }
}
