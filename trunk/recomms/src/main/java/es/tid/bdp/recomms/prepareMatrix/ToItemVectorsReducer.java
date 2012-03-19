package es.tid.bdp.recomms.prepareMatrix;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.mahout.math.VectorWritable;
import org.apache.mahout.math.hadoop.similarity.cooccurrence.Vectors;


import java.io.IOException;

public class ToItemVectorsReducer extends
        Reducer<IntWritable, VectorWritable, IntWritable, VectorWritable> {
    @Override
    protected void reduce(IntWritable row, Iterable<VectorWritable> vectors,
            Context ctx) throws IOException, InterruptedException {
        ctx.write(row, new VectorWritable(Vectors.merge(vectors)));
    }
}
