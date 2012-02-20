package es.tid.ps.mobility.parsing;

import es.tid.ps.mobility.data.MxProtocol.MxCdr;

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
    public MxCdr parse() {
        MxCdr.Builder cdr = MxCdr.newBuilder();

        cdr.setCell(parseCell()); // Cell 1
        cdr.setPhone(parsePhone()); // Phone 1
        if (cdr.getCell() == 0) { // Cell 2
            cdr.setCell(parseCell());
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
