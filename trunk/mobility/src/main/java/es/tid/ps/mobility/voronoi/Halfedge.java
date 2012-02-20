package es.tid.ps.mobility.voronoi;

/**
 *
 * @author dmicol
 */
class Halfedge {
    Halfedge ELleft, ELright;
    Edge ELedge;
    boolean deleted;
    int ELpm;
    Site vertex;
    float ystar;
    Halfedge PQnext;

    Halfedge() {
        this.PQnext = null;
    }
}
