package es.tid.bdp.recomms.matrixMultiply.endMultiplication;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import es.tid.bdp.recomms.data.RecommsProtocol.VectorElement;
import es.tid.bdp.recomms.data.RecommsProtocol.VectorOrPrefs;
import es.tid.bdp.recomms.data.RecommsProtocol.VectorAndPrefs;
import es.tid.bdp.recomms.data.RecommsProtocol.Preference;
import es.tid.bdp.recomms.data.RecommsProtocol.UserId;

import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.io.IntWritable;

public final class ToVectorAndPrefReducer
        extends
        Reducer<IntWritable, ProtobufWritable<VectorOrPrefs>, IntWritable, ProtobufWritable<VectorAndPrefs>> {

    @Override
    public void reduce(IntWritable key,
            Iterable<ProtobufWritable<VectorOrPrefs>> values, Context context)
            throws IOException, InterruptedException {

        ArrayList<Preference> prefValues = new ArrayList<Preference>();
        ArrayList<UserId> userIDs = new ArrayList<UserId>();
        List<VectorElement> vectorList = null;

        ProtobufWritable<VectorAndPrefs> vctrAndPrfsPB = new ProtobufWritable<VectorAndPrefs>();
        vctrAndPrfsPB.setConverter(VectorAndPrefs.class);

        for (ProtobufWritable<VectorOrPrefs> value : values) {
            value.setConverter(VectorOrPrefs.class);
            if (value.get().getVectorList().isEmpty()) {
                // Then this is a user-pref value
                Preference.Builder vpref = Preference.newBuilder();
                UserId.Builder vuid = UserId.newBuilder();

                userIDs.add(vuid.setId(value.get().getUserID()).build());
                prefValues.add(vpref.setPref(value.get().getValue()).build());
            } else {
                // Then this is the column vector
                vectorList = value.get().getVectorList();
            }
        }

        if (vectorList == null) {
            return;
        }

        vctrAndPrfsPB.set(VectorAndPrefs.newBuilder().addAllPrefs(prefValues)
                .addAllUserIds(userIDs).addAllVector(vectorList).build());
        context.write(key, vctrAndPrfsPB);
    }
}
