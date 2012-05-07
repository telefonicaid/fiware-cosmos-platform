package es.tid.cosmos.mobility.voronoi;

/**
 *
 * @author dmicol
 */
public class VoronoiJNI {
    public native boolean generateVoronoi(double[] xValues, double[] yValues,
                                          int numPoints, double minX,
                                          double maxX, double minY,
                                          double maxY, double minDist);
}
