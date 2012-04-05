package es.tid.bdp.mobility.mapreduce;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.bdp.mobility.data.Client;
import es.tid.bdp.mobility.data.MobProtocol.GLEvent;

public class IndividualMobilityReducer extends Reducer<LongWritable,
        ProtobufWritable<GLEvent>, LongWritable, Client> {
    private Client client;
    
    @Override
    protected void setup(final Context context) throws IOException,
                                                       InterruptedException {
        this.client = new Client();
    }

    @Override
    protected void reduce(LongWritable key,
                          Iterable<ProtobufWritable<GLEvent>> values,
                          Context context) throws IOException,
                                                  InterruptedException {
        this.client.clear();
        this.client.setUserId(key.get());
        List<GLEvent> glEvents = new ArrayList<GLEvent>();
        for (ProtobufWritable<GLEvent> wrapper : values) {
            wrapper.setConverter(GLEvent.class);
            glEvents.add(wrapper.get());
        }
        this.client.calculateGeoLocations(glEvents.iterator());
        this.client.calculateNodeCommVector();
        this.client.calculatePoiCommVector();

        context.write(key, this.client);
    }
}
