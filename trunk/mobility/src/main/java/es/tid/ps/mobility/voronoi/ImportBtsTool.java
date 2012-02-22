package es.tid.ps.mobility.voronoi;

import java.io.Reader;
import java.io.Writer;

import es.tid.ps.mobility.data.MobProtocol;
import es.tid.ps.mobility.data.MobProtocol.Bts;

/**
 * Imports BTS information (areas, adjacencies, etc.) into protocol buffers
 * serialized format.
 *
 * @author sortega
 */
public class ImportBtsTool implements Runnable {
    private final Reader input;
    private final Writer output;

    public ImportBtsTool(Reader input, Writer output) {
        this.input = input;
        this.output = output;
    }

    @Override
    public void run() {

        Bts bts = Bts.newBuilder()
                .setPlaceId(1)
                .setPosx(10d)
                .setPosy(20d)
                .setLat(40d)
                .setLon(-3d)
                .setArea(123d)
                .addAllGmtPolygon(bts)
    }

    public static void main(String argv[]) {


    }

}
