package es.tid.smartsteps.dispersion.data;

import java.util.List;

/**
 * FieldsEntry
 *
 * represents a table entry where there are no secondary keys, only field
 * values
 *
 * @author logc
 */
public interface FieldsEntry extends Entry {
    /**
     * Gets all non-key fields in the entry.
     *
     * @return a List of different field Objects
     */
    List<Object> getFields();
}
