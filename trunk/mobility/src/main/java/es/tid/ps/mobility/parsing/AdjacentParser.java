package es.tid.ps.mobility.parsing;

import es.tid.ps.mobility.data.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class AdjacentParser extends PipeDelimitedParser {
    public AdjacentParser(String line) {
        super(line);
    }
    
    @Override
    public TwoInt parse() {
        this.skipField();
        return TwoInt.newBuilder()
                .setNum1(this.parseInt())
                .setNum2(this.parseInt())
                .build();
    }
}
