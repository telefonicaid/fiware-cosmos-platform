package es.tid.bdp.recomms.exporter;

import java.io.IOException;

import com.mongodb.hadoop.io.BSONWritable;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;
import org.bson.BSONObject;
import org.bson.BasicBSONObject;

import es.tid.bdp.recomms.data.RecommsProtocol.Predictions;
import es.tid.bdp.recomms.data.RecommsProtocol.Predictions.Pred;

/**
 * 
 * @author jaume
 * 
 */
public class MongoDBExRecommsReducer extends
        Reducer<Text, ProtobufWritable<Predictions>, Text, BSONWritable> {
    @Override
    public void reduce(Text userId,
            Iterable<ProtobufWritable<Predictions>> recomms, Context context)
            throws IOException, InterruptedException {
        for (ProtobufWritable<Predictions> recomm : recomms) {
            recomm.setConverter(Predictions.class);
            context.write(userId, toBSON(recomm.get()));
        }
    }

    private BSONWritable toBSON(Predictions recomms) {
        BSONObject obj = new BasicBSONObject();
        BSONObject recommsDBObj = new BasicBSONObject();
        for (final Pred pair : recomms.getPredictsList()) {

            recommsDBObj.put(String.valueOf(pair.getItemID()), pair.getValue());
        }
        obj.put("recommendations", recommsDBObj);

        return new BSONWritable(obj);
    }
}
