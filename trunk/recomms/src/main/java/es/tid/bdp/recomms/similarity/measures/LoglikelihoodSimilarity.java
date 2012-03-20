package es.tid.bdp.recomms.similarity.measures;

import org.apache.mahout.math.stats.LogLikelihood;

import es.tid.bdp.recomms.similarity.measures.CountbasedMeasure;

public class LoglikelihoodSimilarity extends CountbasedMeasure {
    @Override
    public double similarity(double summedAggregations, double normA,
            double normB, int numberOfColumns) {
        double logLikelihood = LogLikelihood.logLikelihoodRatio(
                (long) summedAggregations, (long) (normB - summedAggregations),
                (long) (normA - summedAggregations), (long) (numberOfColumns
                        - normA - normB + summedAggregations));

        return 1.0 - 1.0 / (1.0 + logLikelihood);
    }

}
