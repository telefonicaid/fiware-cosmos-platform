package es.tid.ps.mobility.voronoi;

/**
 *
 * @author dmicol
 */
class Freelist {
    Freenode head;

    Freelist() {
        this.head = null;
    }

    public void free() {
        while (this.head != null) {
            this.head = this.head.nextfree;
        }
    }
}
