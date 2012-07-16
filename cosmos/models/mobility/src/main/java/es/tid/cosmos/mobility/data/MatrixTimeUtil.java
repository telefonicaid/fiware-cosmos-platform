package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.generated.BaseProtocol.Date;
import es.tid.cosmos.mobility.data.generated.BaseProtocol.Time;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MatrixTime;

/**
 *
 * @author dmicol
 */
public final class MatrixTimeUtil {

    private MatrixTimeUtil() {}

    public static MatrixTime create(Date date, Time time, int group, long bts) {
        return MatrixTime.newBuilder()
                .setDate(date)
                .setTime(time)
                .setGroup(group)
                .setBts(bts)
                .build();
    }

    public static ProtobufWritable<MatrixTime> wrap(MatrixTime obj) {
        ProtobufWritable<MatrixTime> wrapper = ProtobufWritable.newInstance(
                MatrixTime.class);
        wrapper.set(obj);
        return wrapper;
    }

    public static ProtobufWritable<MatrixTime> createAndWrap(Date date,
            Time time, int group, long bts) {
        return wrap(create(date, time, group, bts));
    }
}
