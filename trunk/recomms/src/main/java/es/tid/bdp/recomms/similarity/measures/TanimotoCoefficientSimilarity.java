package es.tid.bdp.recomms.similarity.measures;

import es.tid.bdp.recomms.similarity.measures.CountbasedMeasure;

public class TanimotoCoefficientSimilarity extends CountbasedMeasure {
    @Override
    public double similarity(double dots, double normA, double normB,
            int numberOfColumns) {
        // Return 0 even when dots == 0 since this will cause it to be ignored
        // -- not NaN
        return dots / (normA + normB - dots);
    }

    @Override
    public boolean consider(int numNonZeroEntriesA, int numNonZeroEntriesB,
            double maxValueA, double maxValueB, double threshold) {
        return numNonZeroEntriesA >= numNonZeroEntriesB * threshold
                && numNonZeroEntriesB >= numNonZeroEntriesA * threshold;
    }
}
