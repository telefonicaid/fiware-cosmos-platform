package es.tid.ps.mobility.voronoi;

/**
 *
 * @author dmicol
 */
class Edge {
    public float a = 0, b = 0, c = 0;
    Site[] ep;
    Site[] reg;
    int edgenbr;

    Edge() {
        this.ep = new Site[2];
        this.reg = new Site[2];
    }
}