package es.tid.cosmos.mobility.mivs;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.TelMonthUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cell;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TelMonth;

/**
 * Input: <TelMonth, Cell>
 * Output: <TelMonth, Cell>
 * 
 * @author logc
 */
public class DeletePeriodMapper extends Mapper<
        ProtobufWritable<TelMonth>, MobilityWritable<Cell>,
        ProtobufWritable<TelMonth>, MobilityWritable<Cell>> {
    private static final int FIRST_MONTH = 1;

    @Override
    public void map(ProtobufWritable<TelMonth> key,
            MobilityWritable<Cell> value, Context context)
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
