package es.tid.bdp.recomms.similarity.measures;

import org.apache.mahout.math.Vector;

import es.tid.bdp.recomms.similarity.measures.VectorSimilarityMeasure;

public abstract class CountbasedMeasure implements VectorSimilarityMeasure {
    @Override
    public Vector normalize(Vector vector) {
        return vector;
    }

    @Override
    public double norm(Vector vector) {
        return vector.norm(0);
    }

    @Override
    public double aggregate(double valueA, double nonZeroValueB) {
        return 1;
    }

    @Override
    public boolean consider(int numNonZeroEntriesA, int numNonZeroEntriesB,
            double maxValueA, double maxValueB, double threshold) {
        return true;
    }
}
