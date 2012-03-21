package es.tid.bdp.recomms.similarity.measures;

import es.tid.bdp.recomms.similarity.measures.CountbasedMeasure;

public class CityBlockSimilarity extends CountbasedMeasure {
    @Override
    public double similarity(double dots, double normA, double normB,
            int numberOfColumns) {
        return 1.0 / (1.0 + normA + normB - 2 * dots);
    }
}
