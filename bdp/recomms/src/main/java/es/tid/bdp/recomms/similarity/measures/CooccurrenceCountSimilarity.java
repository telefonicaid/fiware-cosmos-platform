package es.tid.bdp.recomms.similarity.measures;

import es.tid.bdp.recomms.similarity.measures.CountbasedMeasure;

public class CooccurrenceCountSimilarity extends CountbasedMeasure {
    @Override
    public double similarity(double dots, double normA, double normB,
            int numberOfColumns) {
        return dots;
    }

    @Override
    public boolean consider(int numNonZeroEntriesA, int numNonZeroEntriesB,
            double maxValueA, double maxValueB, double threshold) {
        return numNonZeroEntriesA >= threshold
                && numNonZeroEntriesB >= threshold;
    }
}
