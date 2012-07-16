package es.tid.cosmos.mobility.parsing;

import es.tid.cosmos.mobility.data.generated.MobProtocol.CellGroup;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class CellGroupParser extends Parser {
    public CellGroupParser(String line, String separator) {
        super(line, separator);
    }

    @Override
    public CellGroup parse() {
        try {
            return CellGroup.newBuilder()
                    .setCellId(this.parseCellId())
                    .setGroup(TwoInt.newBuilder()
                            .setNum1(this.parseLong())
                            .setNum2(this.parseLong())
                            .build())
                    .build();
        } catch (Exception ex) {
            throw new IllegalArgumentException("Failed to parse: " + this.line,
                                               ex);
        }
    }
}
