package es.tid.cosmos.mobility.parsing;

import es.tid.cosmos.mobility.data.generated.MobProtocol.Cdr;

/**
 *
 * @author sortega
 */
class CdrParser extends Parser {
    private static final String DELIMITER = "\\|";
    
    public CdrParser(String line) {
        super(line, DELIMITER);
    }

    @Override
    public Cdr parse() {
        try {
            Cdr.Builder cdr = Cdr.newBuilder();

            cdr.setCellId(this.parseCellId()); // Cell 1
            cdr.setUserId(this.parseUserId()); // Phone 1
            if (cdr.getCellId() == 0) { // Cell 2
                cdr.setCellId(this.parseCellId());
            } else {
                this.skipField();
            }
            this.skipField(); // Phone 2
            this.skipField(); // Unknown field

            cdr.setDate(this.parseDate());
            cdr.setTime(this.parseTime());

            return cdr.build();
        } catch (Exception ex) {
            throw new IllegalArgumentException("Failed to parse: " + this.line);
        }
    }
}
