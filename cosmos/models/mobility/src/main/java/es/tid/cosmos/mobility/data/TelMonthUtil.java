package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.generated.MobProtocol.TelMonth;

/**
 *
 * @author dmicol
 */
public abstract class TelMonthUtil {
    public static TelMonth create(long phone, int month, boolean workingday) {
        return TelMonth.newBuilder()
                .setPhone(phone)
                .setMonth(month)
                .setWorkingday(workingday)
                .build();
    }

    public static ProtobufWritable<TelMonth> wrap(TelMonth obj) {
        ProtobufWritable<TelMonth> wrapper = ProtobufWritable.newInstance(
                TelMonth.class);
        wrapper.set(obj);
        return wrapper;
    }
    
    public static ProtobufWritable<TelMonth> createAndWrap(long phone,
                                                           int month,
                                                           boolean workingday) {
        return wrap(create(phone, month, workingday));
    }
}
