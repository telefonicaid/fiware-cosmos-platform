package es.tid.smartsteps.footfalls.centroids;

import es.tid.smartsteps.footfalls.data.generated.EntryProtocol.SOACentroid;
import es.tid.smartsteps.footfalls.parsing.Parser;
import es.tid.smartsteps.footfalls.parsing.ParserUtil;

/**
 *
 * @author dmicol
 */
public class CentroidParser extends Parser<SOACentroid> {

    public CentroidParser(String delimiter) {
        super(delimiter);
    }

    @Override
    public SOACentroid parse(String line) {
        final String[] values = line.split(this.delimiter);
        if (values.length != 4) {
            return null;
        }
        return SOACentroid.newBuilder()
                .setSoaId(ParserUtil.safeUnquote(values[0]))
                .setSquaredKilometers(Double.parseDouble(values[1]))
                .setLongitude(Double.parseDouble(values[2]))
                .setLatitude(Double.parseDouble(values[3]))
                .build();
    }
}
