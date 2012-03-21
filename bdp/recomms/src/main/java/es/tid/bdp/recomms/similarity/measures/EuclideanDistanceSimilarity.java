package es.tid.bdp.recomms.similarity.measures;

import org.apache.mahout.math.Vector;

import es.tid.bdp.recomms.similarity.measures.VectorSimilarityMeasure;

import java.util.Iterator;

public class EuclideanDistanceSimilarity implements VectorSimilarityMeasure {
    @Override
    public Vector normalize(Vector vector) {
        return vector;
    }

    @Override
    public double norm(Vector vector) {
        double norm = 0;
        Iterator<Vector.Element> nonZeroElements = vector.iterateNonZero();
        while (nonZeroElements.hasNext()) {
            double value = nonZeroElements.next().get();
            norm += value * value;
        }
        return norm;
    }

    @Override
    public double aggregate(double valueA, double nonZeroValueB) {
        return valueA * nonZeroValueB;
    }

    @Override
    public double similarity(double dots, double normA, double normB,
            int numberOfColumns) {
        double euclideanDistance = Math.sqrt(normA - 2 * dots + normB);
        return 1.0 / (1.0 + euclideanDistance);
    }

    @Override
    public boolean consider(int numNonZeroEntriesA, int numNonZeroEntriesB,
            double maxValueA, double maxValueB, double threshold) {
        return true;
    }
}
