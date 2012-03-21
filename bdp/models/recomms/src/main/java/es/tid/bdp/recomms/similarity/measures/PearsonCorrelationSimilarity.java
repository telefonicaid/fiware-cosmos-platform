package es.tid.bdp.recomms.similarity.measures;

import org.apache.mahout.math.Vector;

import es.tid.bdp.recomms.similarity.measures.CosineSimilarity;

import java.util.Iterator;

public class PearsonCorrelationSimilarity extends CosineSimilarity {
    @Override
    public Vector normalize(Vector vector) {
        if (vector.getNumNondefaultElements() == 0) {
            return vector;
        }
        // center non-zero elements
        double average = vector.norm(1) / vector.getNumNondefaultElements();

        Iterator<Vector.Element> nonZeroElements = vector.iterateNonZero();

        while (nonZeroElements.hasNext()) {
            Vector.Element nonZeroElement = nonZeroElements.next();
            double norm = nonZeroElement.get() - average;
            vector.setQuick(nonZeroElement.index(), norm);
        }
        return super.normalize(vector);
    }
}
