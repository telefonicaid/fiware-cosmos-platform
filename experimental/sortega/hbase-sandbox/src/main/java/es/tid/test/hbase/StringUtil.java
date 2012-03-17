package es.tid.test.hbase;

import java.util.Iterator;

/**
 *
 * @author sortega
 */
public class StringUtil {

    public static <T> String join(String separator, Iterable<T> values) {
        Iterator<T> it = values.iterator();
        String str = "";
        if (it.hasNext()) {
            str += it.next();
        }
        while (it.hasNext()) {
            str += separator + it.next();
        }
        return str;
    }

    private StringUtil() {
    }
}
