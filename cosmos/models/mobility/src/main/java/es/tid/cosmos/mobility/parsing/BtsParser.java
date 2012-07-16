package es.tid.cosmos.mobility.parsing;

import es.tid.cosmos.mobility.data.generated.MobProtocol.Bts;

/**
 *
 * @author dmicol
 */
public class BtsParser extends Parser {
    public BtsParser(String line, String separator) {
        super(line, separator);
    }

    @Override
    public Bts parse() {
        try {
            Bts.Builder bts = Bts.newBuilder();
            bts.setPlaceId(this.parseLong());
            this.nextToken();
            bts.setArea(this.parseDouble());
            bts.setComms(this.parseInt());
            return bts.build();
        } catch (Exception ex) {
            throw new IllegalArgumentException("Failed to parse: " + this.line);
        }
    }
}
