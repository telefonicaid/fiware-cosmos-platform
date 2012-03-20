package es.tid.bdp.recomms.similarity.measures;

import org.apache.mahout.math.Vector;

public interface VectorSimilarityMeasure {

    double NO_NORM = 0.0;

    Vector normalize(Vector vector);

    double norm(Vector vector);

    double aggregate(double nonZeroValueA, double nonZeroValueB);

    double similarity(double summedAggregations, double normA, double normB,
            int numberOfColumns);

    boolean consider(int numNonZeroEntriesA, int numNonZeroEntriesB,
            double maxValueA, double maxValueB, double threshold);
}
