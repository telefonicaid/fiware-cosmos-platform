package es.tid.cosmos.mobility.parsing;

import es.tid.cosmos.mobility.data.MobProtocol.Cdr;

/**
 *
 * @author sortega
 */
public class CdrParser extends PipeDelimitedParser {
    public CdrParser(String line) {
        super(line);
    }

    //LINE --> "33F430521676F4|2221436242|33F430521676F4|0442224173253|2|01/01/2010|02:00:01|2891|RMITERR"
    @Override
    public Cdr parse() {
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
    }
}
