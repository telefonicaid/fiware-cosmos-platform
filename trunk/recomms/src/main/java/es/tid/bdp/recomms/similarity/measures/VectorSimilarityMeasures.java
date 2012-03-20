package es.tid.bdp.recomms.similarity.measures;

import java.util.Arrays;

import es.tid.bdp.recomms.similarity.measures.CityBlockSimilarity;
import es.tid.bdp.recomms.similarity.measures.CooccurrenceCountSimilarity;
import es.tid.bdp.recomms.similarity.measures.CosineSimilarity;
import es.tid.bdp.recomms.similarity.measures.EuclideanDistanceSimilarity;
import es.tid.bdp.recomms.similarity.measures.LoglikelihoodSimilarity;
import es.tid.bdp.recomms.similarity.measures.PearsonCorrelationSimilarity;
import es.tid.bdp.recomms.similarity.measures.TanimotoCoefficientSimilarity;
import es.tid.bdp.recomms.similarity.measures.VectorSimilarityMeasure;

public enum VectorSimilarityMeasures {

    SIMILARITY_COOCCURRENCE(CooccurrenceCountSimilarity.class), SIMILARITY_LOGLIKELIHOOD(
            LoglikelihoodSimilarity.class), SIMILARITY_TANIMOTO_COEFFICIENT(
            TanimotoCoefficientSimilarity.class), SIMILARITY_CITY_BLOCK(
            CityBlockSimilarity.class), SIMILARITY_COSINE(
            CosineSimilarity.class), SIMILARITY_PEARSON_CORRELATION(
            PearsonCorrelationSimilarity.class), SIMILARITY_EUCLIDEAN_DISTANCE(
            EuclideanDistanceSimilarity.class);

    private final Class<? extends VectorSimilarityMeasure> implementingClass;

    VectorSimilarityMeasures(Class<? extends VectorSimilarityMeasure> impl) {
        this.implementingClass = impl;
    }

    public String getClassname() {
        return implementingClass.getName();
    }

    public static String list() {
        return Arrays.toString(values());
    }

}
