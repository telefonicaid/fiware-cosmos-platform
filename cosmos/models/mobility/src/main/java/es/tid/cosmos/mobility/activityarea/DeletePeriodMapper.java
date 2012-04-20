package es.tid.cosmos.mobility.activityarea;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.TelMonthUtil;
import es.tid.cosmos.mobility.data.MobProtocol.Cell;
import es.tid.cosmos.mobility.data.MobProtocol.TelMonth;

/**
 *
 * @author losa
 */
public class DeletePeriodMapper extends Mapper<
        ProtobufWritable<TelMonth>, ProtobufWritable<Cell>,
        ProtobufWritable<TelMonth>, ProtobufWritable<Cell>> {
    private static final int FIRST_MONTH = 1;

    @Override
    public void map(ProtobufWritable<TelMonth> key,
            ProtobufWritable<Cell> value, Context context)
            throws IOException, InterruptedException {
        key.setConverter(TelMonth.class);
        final TelMonth oldKey = key.get();
        ProtobufWritable<TelMonth> newKey =
            TelMonthUtil.createAndWrap(oldKey.getPhone(),
                                       FIRST_MONTH,
                                       oldKey.getWorkingday());
        context.write(newKey, value);
    }
}
