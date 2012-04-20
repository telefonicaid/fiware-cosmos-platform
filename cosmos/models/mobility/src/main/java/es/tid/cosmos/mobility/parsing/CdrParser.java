package es.tid.cosmos.mobility.parsing;

import es.tid.cosmos.mobility.data.MobProtocol.Cdr;

/**
 *
 * @author sortega
 */
public class CdrParser extends Parser {
    private static final String DELIMITER = "\\|";
    
    public CdrParser(String line) {
        super(line, DELIMITER);
    }

    //LINE --> "33F430521676F4|2221436242|33F430521676F4|0442224173253|2|01/01/2010|02:00:01|2891|RMITERR"
    @Override
    public Cdr parse() {
        try {
            Cdr.Builder cdr = Cdr.newBuilder();

            cdr.setCellId(parseCellId()); // Cell 1
            cdr.setUserId(parseUserId()); // Phone 1
            if (cdr.getCellId() == 0) { // Cell 2
                cdr.setCellId(parseCellId());
            } else {
                skipField();
            }
            skipField(); // Phone 2
            skipField(); // Unknown field

            cdr.setDate(parseDate());
            cdr.setTime(parseTime());

            return cdr.build();
        } catch (Exception ex) {
            throw new IllegalArgumentException("Failed to parse: " + this.line);
        }
    }
}
