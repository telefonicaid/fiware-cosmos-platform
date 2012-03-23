package es.tid.bdp.recomms.extractRecomms;

import java.io.IOException;
import java.util.List;

import org.apache.hadoop.mapreduce.Mapper;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.IntWritable;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.bdp.recomms.data.RecommsProtocol.PrefandSimil;
import es.tid.bdp.recomms.data.RecommsProtocol.VectorAndPrefs;
import es.tid.bdp.recomms.data.RecommsProtocol.Preference;
import es.tid.bdp.recomms.data.RecommsProtocol.UserId;
import es.tid.bdp.recomms.data.RecommsProtocol.VectorElement;

/**
 * maps similar items and their preference values per user
 */
public final class PartialMultiplyMapper
        extends
        Mapper<IntWritable, ProtobufWritable<VectorAndPrefs>, LongWritable, ProtobufWritable<PrefandSimil>> {
    @Override
    public void map(IntWritable key,
            ProtobufWritable<VectorAndPrefs> vectorAndPrefsPT, Context context)
            throws IOException, InterruptedException {

        vectorAndPrefsPT.setConverter(VectorAndPrefs.class);

        List<VectorElement> similarityMatrixColumn = vectorAndPrefsPT.get()
                .getVectorList();
        List<UserId> userIDs = vectorAndPrefsPT.get().getUserIdsList();
        List<Preference> prefValues = vectorAndPrefsPT.get().getPrefsList();

        LongWritable userIDWritable = new LongWritable();
        ProtobufWritable<PrefandSimil> prefAndSimilarityColumn = new ProtobufWritable<PrefandSimil>();
        prefAndSimilarityColumn.setConverter(PrefandSimil.class);

        for (int i = 0; i < userIDs.size(); i++) {
            UserId userID = userIDs.get(i);
            Preference prefValue = prefValues.get(i);
            if (!Float.isNaN(prefValue.getPref())) {
                prefAndSimilarityColumn.set(PrefandSimil.newBuilder()
                        .setPrefvalue(prefValue.getPref())
                        .addAllVector(similarityMatrixColumn).build());
                userIDWritable.set(userID.getId());
                context.write(userIDWritable, prefAndSimilarityColumn);
            }
        }
    }

}