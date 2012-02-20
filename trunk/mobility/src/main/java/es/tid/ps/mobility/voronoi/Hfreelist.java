package es.tid.ps.mobility.voronoi;

/**
 *
 * @author dmicol
 */
class Hfreelist extends Freelist {
    Halfedge hfl;

    Hfreelist() {
        this.hfl = new Halfedge();
    }
}