package es.tid.bdp.mobility.data;

/**
 *
 * @author dmicol
 */
public final class CellCatalogueFactory {
    private static CellCatalogue instance = null;
    
    private CellCatalogueFactory() {
    }

    public static CellCatalogue getInstance() {
        if (instance == null) {
            createInstance();
        }
        return instance;
    }

    private static synchronized void createInstance() {
        instance = new CellCatalogue();
    }
}
