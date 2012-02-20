package es.tid.ps.mobility.voronoi;

/**
 *
 * @author dmicol
 */
class Freenode {
    Freenode nextfree;

    Freenode() {
    }
    
    Freenode getnext() {
        return this.nextfree;
    }

    public void setnext(Freenode newf) {
        this.nextfree = new Freenode();
        this.nextfree = newf;
    }
}
