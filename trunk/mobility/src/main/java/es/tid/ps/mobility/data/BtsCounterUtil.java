package es.tid.ps.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.ps.mobility.data.MxProtocol.BtsCounter;

/**
 *
 * @author sortega
 */
public abstract class BtsCounterUtil implements ProtobufUtil {
    public static BtsCounter create(int bts, int wday, int range, int count) {
        return BtsCounter.newBuilder()
                .setBts(bts)
                .setWday(wday)
                .setRange(range)
                .setCount(count)
                .build();
    }

    public static ProtobufWritable<BtsCounter> createAndWrap(int bts, int wday,
            int range, int count) {
        ProtobufWritable<BtsCounter> wrapper =
                ProtobufWritable.newInstance(BtsCounter.class);
        wrapper.set(create(bts, wday, range, count));
        return wrapper;
    }
}