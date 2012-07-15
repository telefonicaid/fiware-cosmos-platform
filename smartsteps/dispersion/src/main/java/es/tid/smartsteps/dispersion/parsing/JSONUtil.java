package es.tid.smartsteps.dispersion.parsing;

import java.util.StringTokenizer;

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
     * @param json
     * @param property    Property name (dot-delimited nested-field names)
     * @return            The value or null
     * @throws IllegalArgumentException
     *                    If the property name is empty or a subproperty of a
     *                    type different from JSONObject or JSONArray is
     *                    requested.
     */
    public static Object getProperty(Object json, String property) {
        final StringTokenizer fieldNames =
                new StringTokenizer(property, FIELD_DELIMITER);
        if (!fieldNames.hasMoreElements()) {
            throw new IllegalArgumentException("Empty property name");
        }

        Object value = json;
        while (fieldNames.hasMoreElements()) {
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
     * @param object      JSON value to update
     * @param property    Property name (dot-delimited nested-field names)
     * @param value       Value to set
     * @throws IllegalArgumentException
     *                    If the property name is empty or a subproperty of a
     *                    type different from JSONObject or JSONArray is
     *                    accessed.
     */
    public static void setProperty(Object object, String property, Object value) {
        final StringTokenizer fieldNames =
                new StringTokenizer(property, FIELD_DELIMITER);
        if (!fieldNames.hasMoreElements()) {
            throw new IllegalArgumentException("Empty property name");
        }

        String fieldName = fieldNames.nextToken();

        while (fieldNames.hasMoreElements()) {
            if (object instanceof JSONObject) {
                JSONObject jsonObject = (JSONObject) object;
                if (!jsonObject.containsKey(fieldName)) {
                    jsonObject.put(fieldName, new JSONObject());
                }
                object = jsonObject.get(fieldName);

            } else if (value instanceof JSONArray) {
                int index = Integer.parseInt(fieldName);
                JSONArray jsonArray = (JSONArray) object;
                if (jsonArray.get(index) == null) {
                    jsonArray.element(index, new JSONObject());
                }
                value = jsonArray.get(index);

            } else {
                throwInvalidNest(fieldName, value);
            }

            fieldName = fieldNames.nextToken();
        }

        if (object instanceof JSONObject) {
            ((JSONObject) object).put(fieldName, value);
        } else if (object instanceof JSONArray) {
            ((JSONArray) object).element(Integer.parseInt(fieldName), value);
        } else {
            throwInvalidNest(fieldName, value);
        }
    }

    private static void throwInvalidNest(String fieldName, Object value) {
        throw new IllegalArgumentException(String.format(
                    "Cannot access nested property '%s' on a value of class %s",
                    fieldName, value.getClass()));
    }
}
