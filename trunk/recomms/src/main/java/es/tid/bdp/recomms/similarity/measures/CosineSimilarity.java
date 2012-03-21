package es.tid.bdp.recomms.similarity.measures;

import org.apache.mahout.math.Vector;

import es.tid.bdp.recomms.similarity.measures.VectorSimilarityMeasure;

public class CosineSimilarity implements VectorSimilarityMeasure {
    @Override
    public Vector normalize(Vector vector) {
        return vector.normalize();
    }

    @Override
    public double norm(Vector vector) {
        return VectorSimilarityMeasure.NO_NORM;
    }

    @Override
    public double aggregate(double valueA, double nonZeroValueB) {
        return valueA * nonZeroValueB;
    }

    @Override
    public double similarity(double dots, double normA, double normB,
            int numberOfColumns) {
        return dots;
    }

    @Override
    public boolean consider(int numNonZeroEntriesA, int numNonZeroEntriesB,
            double maxValueA, double maxValueB, double threshold) {
        return numNonZeroEntriesB >= threshold / maxValueA
                && numNonZeroEntriesA >= threshold / maxValueB;
    }
}
