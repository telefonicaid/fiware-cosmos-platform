package es.tid.smartsteps.footfalls.parsing;

import java.util.StringTokenizer;

import net.sf.json.JSON;
import net.sf.json.JSONArray;
import net.sf.json.JSONObject;

/**
 * Utility functions to manipulate JSON values.
 *
 * @author sortega
 */
public final class JSONUtil {

    private static final String FIELD_DELIMITER = ".";

    private JSONUtil() {}

    /**
     * Beans-like getter method that supports the dot character to access
     * nested fields.
     *
     * Note that arrays can be accessed with the same notation, e.g. "array.0"
     * would access the first array element.
     *
     * @param json        JSON object or array
     * @param property    Property name (dot-delimited nested-field names)
     * @return            The value or null
     * @throws IllegalArgumentException
     *                    If the property name is empty or a subproperty of a
     *                    type different from JSONObject or JSONArray is
     *                    requested.
     */
    public static Object getProperty(JSON json, String property) {
        final StringTokenizer fieldNames =
                new StringTokenizer(property, FIELD_DELIMITER);
        if (!fieldNames.hasMoreTokens()) {
            throw new IllegalArgumentException("Empty property name");
        }

        Object value = json;
        while (fieldNames.hasMoreTokens()) {
            final String fieldName = fieldNames.nextToken();
            if (value == null) {
                return null;
            } else if (value instanceof JSONObject) {
                value = ((JSONObject) value).get(fieldName);
            } else if (value instanceof JSONArray) {
                int index = Integer.parseInt(fieldName);
                value = ((JSONArray) value).get(index);
            } else {
                throwInvalidNest(fieldName, value);
            }
        }
        return value;
    }

    /**
     * Beans-like setter method that supports the dot character to access
     * nested fields.
     *
     * Note that arrays can be accessed with the same notation, e.g. "array.0"
     * would access the first array element.
     *
     * @param object      JSON value to update
     * @param property    Property name (dot-delimited nested-field names)
     * @param value       Value to set
     * @throws IllegalArgumentException
     *                    If the property name is empty or a subproperty of a
     *                    type different from JSONObject or JSONArray is
     *                    accessed.
     */
    public static void setProperty(JSON object, String property,
                                   Object value) {
        final StringTokenizer fieldNames =
                new StringTokenizer(property, FIELD_DELIMITER);
        if (!fieldNames.hasMoreTokens()) {
            throw new IllegalArgumentException("Empty property name");
        }

        Object lvalue = object;
        String fieldName = fieldNames.nextToken();
        while (fieldNames.hasMoreTokens()) {
            if (lvalue instanceof JSONObject) {
                JSONObject jsonObject = (JSONObject) lvalue;
                if (!jsonObject.containsKey(fieldName)) {
                    jsonObject.put(fieldName, new JSONObject());
                }
                lvalue = jsonObject.get(fieldName);

            } else if (lvalue instanceof JSONArray) {
                int index = Integer.parseInt(fieldName);
                JSONArray jsonArray = (JSONArray) lvalue;
                if (jsonArray.get(index) == null) {
                    jsonArray.element(index, new JSONObject());
                }
                lvalue = jsonArray.get(index);

            } else {
                throwInvalidNest(fieldName, value);
            }

            fieldName = fieldNames.nextToken();
        }

        if (lvalue instanceof JSONObject) {
            ((JSONObject) lvalue).put(fieldName, value);
        } else if (lvalue instanceof JSONArray) {
            ((JSONArray) lvalue).element(Integer.parseInt(fieldName), value);
        } else {
            throwInvalidNest(fieldName, lvalue);
        }
    }

    private static void throwInvalidNest(String fieldName, Object value) {
        throw new IllegalArgumentException(String.format(
                "Cannot access nested property '%s' on a value of class %s",
                fieldName, value.getClass()));
    }
}
