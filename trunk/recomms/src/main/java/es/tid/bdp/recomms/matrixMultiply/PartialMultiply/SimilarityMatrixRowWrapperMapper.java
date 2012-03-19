package es.tid.bdp.recomms.matrixMultiply.PartialMultiply;

import java.io.IOException;
import java.util.Iterator;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.mahout.math.VectorWritable;
import org.apache.mahout.math.Vector;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.bdp.recomms.data.RecommsProtocol.VectorOrPrefs;
import es.tid.bdp.recomms.data.RecommsProtocol.VectorElement;

/**
 * maps a row of the similarity matrix to a {@link VectorOrPrefWritable}
 * 
 * actually a column from that matrix has to be used but as the similarity
 * matrix is symmetric, we can use a row instead of having to transpose it
 */
public final class SimilarityMatrixRowWrapperMapper
        extends
        Mapper<IntWritable, VectorWritable, IntWritable, ProtobufWritable<VectorOrPrefs>> {

    @Override
    protected void map(IntWritable key, VectorWritable value, Context context)
            throws IOException, InterruptedException {
        Vector similarityMatrixRow = value.get();
        /* remove self similarity */
        similarityMatrixRow.set(key.get(), Double.NaN);

        ProtobufWritable<VectorOrPrefs> vectorOrPref = new ProtobufWritable<VectorOrPrefs>();
        VectorOrPrefs.Builder voPref = VectorOrPrefs.newBuilder();
        vectorOrPref.setConverter(VectorOrPrefs.class);

        Iterator<Vector.Element> vectorElements = value.get().iterateNonZero();
        while (vectorElements.hasNext()) {
            Vector.Element elem = vectorElements.next();
            VectorElement.Builder vElem = VectorElement.newBuilder();

            vElem.setValue(elem.get()).setIndex(elem.index());
            voPref.addVector(vElem);
        }

        vectorOrPref.set(voPref.build());
        context.write(new IntWritable(key.get()), vectorOrPref);
    }

}
