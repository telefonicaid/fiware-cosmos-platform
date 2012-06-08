package es.tid.cosmos.mobility.data;

import com.google.protobuf.Message;
import com.twitter.elephantbird.mapreduce.io.BinaryConverter;
import com.twitter.elephantbird.mapreduce.io.BinaryWritable;

/**
 *
 * @author ximo
 */
public class MobilityWritable<M extends Message> extends BinaryWritable<M> {
    public MobilityWritable() {
        super(null, new MobilityConverter(Message.class));
    }
    
    public MobilityWritable(M obj, Class<M> clazz) {
        super(obj, new MobilityConverter(clazz));
    }
    
    @Override
    protected BinaryConverter<M> getConverterFor(Class<M> clazz) {
        return new MobilityConverter(clazz);
    }
}
