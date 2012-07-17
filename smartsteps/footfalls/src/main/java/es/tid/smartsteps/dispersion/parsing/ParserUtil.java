package es.tid.smartsteps.dispersion.parsing;

/**
 * Utilities for parsing text files.
 *
 * @author sortega
 */
public final class ParserUtil {

    private ParserUtil() {}

    public static String safeUnquote(String field) {
        if (isQuoted(field)) {
            return field.substring(1, field.length() - 1);
        }
        return field;
    }

    public static boolean isQuoted(String field) {
        return field.startsWith("\"") && field.endsWith("\"");
    }
}
