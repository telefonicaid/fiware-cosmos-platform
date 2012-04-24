package es.tid.cosmos.mobility.mivs;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.TelMonthUtil;
import es.tid.cosmos.mobility.data.MobProtocol.Cell;
import es.tid.cosmos.mobility.data.MobProtocol.TelMonth;

/**
 *
 * @author logc
 */
public class DeletePeriodMapper extends Mapper<
        ProtobufWritable<TelMonth>, ProtobufWritable<Cell>,
        ProtobufWritable<TelMonth>, ProtobufWritable<Cell>> {
    private static final int JANUARY = 1;

    @Override
    public void map(ProtobufWritable<TelMonth> key,
            ProtobufWritable<Cell> value, Context context)
            throws IOException, InterruptedException {
        key.setConverter(TelMonth.class);
        final TelMonth oldKey = key.get();
        ProtobufWritable<TelMonth> newKey =
                TelMonthUtil.createAndWrap(oldKey.getPhone(),
                                           JANUARY,
                                           oldKey.getWorkingday());
        context.write(newKey, value);
    }
}
