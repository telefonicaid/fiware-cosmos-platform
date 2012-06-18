package es.tid.smartsteps.ipm;

/**
 *
 * @author dmicol
 */
public interface RawToIpmConverter {
    /**
     * Converts a string that represents a raw record to a string that 
     * represents the IPM output of such record.
     * @param line The input line.
     * @return The output record textual representation.
     * @throws ParseException when input line cannot be parsed
     */
    String convert(String line) throws ParseException;
}
