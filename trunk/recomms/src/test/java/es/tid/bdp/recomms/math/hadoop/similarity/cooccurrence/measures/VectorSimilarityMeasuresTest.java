package es.tid.bdp.recomms.math.hadoop.similarity.cooccurrence.measures;

import org.apache.mahout.common.ClassUtils;
import org.apache.mahout.math.RandomAccessSparseVector;
import org.apache.mahout.math.Vector;
import org.apache.mahout.common.MahoutTestCase;
import org.junit.Test;

import es.tid.bdp.recomms.similarity.measures.CityBlockSimilarity;
import es.tid.bdp.recomms.similarity.measures.CooccurrenceCountSimilarity;
import es.tid.bdp.recomms.similarity.measures.CosineSimilarity;
import es.tid.bdp.recomms.similarity.measures.EuclideanDistanceSimilarity;
import es.tid.bdp.recomms.similarity.measures.LoglikelihoodSimilarity;
import es.tid.bdp.recomms.similarity.measures.PearsonCorrelationSimilarity;
import es.tid.bdp.recomms.similarity.measures.TanimotoCoefficientSimilarity;
import es.tid.bdp.recomms.similarity.measures.VectorSimilarityMeasure;

public class VectorSimilarityMeasuresTest extends MahoutTestCase {

    static double distributedSimilarity(double[] one, double[] two,
            Class<? extends VectorSimilarityMeasure> similarityMeasureClass) {
        VectorSimilarityMeasure similarityMeasure = ClassUtils.instantiateAs(
                similarityMeasureClass, VectorSimilarityMeasure.class);
        Vector oneNormalized = similarityMeasure.normalize(asSparseVector(one));
        Vector twoNormalized = similarityMeasure.normalize(asSparseVector(two));

        double normOne = similarityMeasure.norm(oneNormalized);
        double normTwo = similarityMeasure.norm(twoNormalized);

        double dot = 0;
        for (int n = 0; n < one.length; n++) {
            if (oneNormalized.get(n) != 0 && twoNormalized.get(n) != 0) {
                dot += similarityMeasure.aggregate(oneNormalized.get(n),
                        twoNormalized.get(n));
            }
        }
        return similarityMeasure.similarity(dot, normOne, normTwo, one.length);
    }

    static Vector asSparseVector(double[] values) {
        Vector vector = new RandomAccessSparseVector(Integer.MAX_VALUE);
        for (int dim = 0; dim < values.length; dim++) {
            if (values[dim] != 0) {
                vector.setQuick(dim, values[dim]);
            }
        }
        return vector;
    }

    @Test
    public void testCooccurrenceCountSimilarity() {
        double similarity = distributedSimilarity(new double[] { 0, 1, 0, 0, 1,
                1, 0, 1, 0, 1, 1, 1, 0 }, new double[] { 1, 0, 0, 0, 1, 0, 1,
                1, 1, 1, 1, 1, 1 }, CooccurrenceCountSimilarity.class);

        assertEquals(5.0, similarity, 0);
    }

    @Test
    public void testTanimotoCoefficientSimilarity() {
        double similarity = distributedSimilarity(new double[] { 0, 1, 0, 0, 1,
                1, 0, 1, 0, 1, 1, 1, 0 }, new double[] { 1, 0, 0, 0, 1, 0, 1,
                1, 1, 1, 1, 1, 1 }, TanimotoCoefficientSimilarity.class);

        assertEquals(0.454545455, similarity, EPSILON);
    }

    @Test
    public void testCityblockSimilarity() {
        double similarity = distributedSimilarity(new double[] { 0, 1, 0, 0, 1,
                1, 0, 1, 0, 1, 1, 1, 0 }, new double[] { 1, 0, 0, 0, 1, 0, 1,
                1, 1, 1, 1, 1, 1 }, CityBlockSimilarity.class);

        assertEquals(0.142857143, similarity, EPSILON);
    }

    @Test
    public void testLoglikelihoodSimilarity() {
        double similarity = distributedSimilarity(new double[] { 0, 1, 0, 0, 1,
                1, 0, 1, 0, 1, 1, 1, 0 }, new double[] { 1, 0, 0, 0, 1, 0, 1,
                1, 1, 1, 1, 1, 1 }, LoglikelihoodSimilarity.class);

        assertEquals(0.03320155369284261, similarity, EPSILON);
    }

    @Test
    public void testCosineSimilarity() {
        double similarity = distributedSimilarity(new double[] { 0, 2, 0, 0, 8,
                3, 0, 6, 0, 1, 2, 2, 0 }, new double[] { 3, 0, 0, 0, 7, 0, 2,
                2, 1, 3, 2, 1, 1 }, CosineSimilarity.class);

        assertEquals(0.769846046, similarity, EPSILON);
    }

    @Test
    public void testPearsonCorrelationSimilarity() {
        double similarity = distributedSimilarity(new double[] { 0, 2, 0, 0, 8,
                3, 0, 6, 0, 1, 1, 2, 1 }, new double[] { 3, 0, 0, 0, 7, 0, 2,
                2, 1, 3, 2, 4, 3 }, PearsonCorrelationSimilarity.class);

        // System.out.println(EPSILON);
        // System.out.println(similarity);
        assertEquals(0.5303300858899108, similarity, EPSILON);
        // assertEquals(0.7483314773547882, similarity, EPSILON);
    }

    @Test
    public void testEuclideanDistanceSimilarity() {
        double similarity = distributedSimilarity(new double[] { 0, 2, 0, 0, 8,
                3, 0, 6, 0, 1, 1, 2, 1 }, new double[] { 3, 0, 0, 0, 7, 0, 2,
                2, 1, 3, 2, 4, 4 }, EuclideanDistanceSimilarity.class);

        assertEquals(0.11268865367232477, similarity, EPSILON);
    }
}
