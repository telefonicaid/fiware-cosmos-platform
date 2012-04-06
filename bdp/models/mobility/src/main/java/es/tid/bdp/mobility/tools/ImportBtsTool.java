package es.tid.bdp.mobility.tools;

import java.io.*;
import java.util.Map;
import java.util.TreeMap;

import es.tid.bdp.mobility.data.MobProtocol.Bts;
import es.tid.bdp.mobility.data.MobProtocol.Bts.Builder;

/**
 * Imports BTS information (areas, adjacencies, etc.) into protocol buffers
 * serialized format.
 *
 * @author sortega
 */
public class ImportBtsTool {
    private static final String SEPARATOR = "\\|";
    private final OutputStream output;
    private final Map<Long, Bts.Builder> btss;

    public static void main(String argv[]) {
        if (argv.length != 4) {
            System.err.println("Usage: command cell_catalogue adjacent_bts "
                    + "areas out_file");
            System.exit(1);
        }

        OutputStream output = null;
        try {
            Reader cellCatalogue = openReader(argv[0]);
            Reader adjacentBts = openReader(argv[1]);
            Reader areas = openReader(argv[2]);
            output = new FileOutputStream(argv[3]);
            new ImportBtsTool(cellCatalogue, adjacentBts, areas, output).run();
            System.exit(0);
        } catch (IOException ex) {
            System.err.println("I/O problem: " + ex.getMessage());
            System.exit(1);
        } finally {
            try {
                output.close();
            } catch (IOException ignored) {
                // Do nothing
            }
        }
    }

    public static Reader openReader(String filename) {
        try {
            return new FileReader(filename);
        } catch (FileNotFoundException fnfe) {
            System.err.println("File not found: " + filename);
            System.exit(1);
            return null;
        }
    }
    private final Reader cellCatalogue;
    private final Reader adjacentBts;
    private final Reader areas;

    public ImportBtsTool(Reader cellCatalogue,
            Reader adjacentBts,
            Reader areas,
            OutputStream output) {
        this.cellCatalogue = cellCatalogue;
        this.adjacentBts = adjacentBts;
        this.areas = areas;
        this.output = output;
        this.btss = new TreeMap<Long, Bts.Builder>();
    }

    public void run() throws IOException {
        this.loadCellCatalogue();
        this.addAdjacentBts();
        this.addAreas();
        this.serializeBtss();
    }

    private void serializeBtss() throws IOException {
        for (Bts.Builder bts : btss.values()) {
            bts.build().writeDelimitedTo(output);
        }
    }

    private void loadCellCatalogue() throws IOException {
        BufferedReader lines = new BufferedReader(cellCatalogue);
        String line;
        while ((line = lines.readLine()) != null) {
            String[] fields = line.split(SEPARATOR);
            long placeId = Long.parseLong(fields[0]);
            double posx = Long.parseLong(fields[1]);
            double posy = Long.parseLong(fields[2]);

            btss.put(placeId, Bts.newBuilder().setPlaceId(placeId).setPosx(posx).
                    setPosy(posy));
        }
    }

    private void addAdjacentBts() throws IOException {
        BufferedReader lines = new BufferedReader(adjacentBts);
        String line;
        while ((line = lines.readLine()) != null) {
            String[] fields = line.split(SEPARATOR);
            long fromPlace = Long.parseLong(fields[0]);
            long toPlace = Long.parseLong(fields[1]);

            if (btss.containsKey(fromPlace) && btss.containsKey(toPlace)) {
                btss.get(fromPlace).addAdjBts(toPlace);
            }
        }
    }

    private void addAreas() throws IOException {
        BufferedReader lines = new BufferedReader(areas);
        String line;
        while ((line = lines.readLine()) != null) {
            String[] fields = line.split(SEPARATOR);
            long placeId = Long.parseLong(fields[0]);
            double area = Double.parseDouble(fields[2]);
            Builder bts = btss.get(placeId);
            if (bts != null) {
                bts.setArea(area);
            }
        }
    }
}
