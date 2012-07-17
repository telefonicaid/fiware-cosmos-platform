package es.tid.smartsteps.footfalls.microgrids;

import java.io.IOException;

import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.mapreduce.BinaryKey;
import es.tid.smartsteps.footfalls.microgrids.data.generated.EntryProtocol.TrafficCounts;

/**
 *
 * @author dmicol
 */
class CellIdAndDateMapper extends Mapper<
        Text, TypedProtobufWritable<TrafficCounts>,
        BinaryKey, TypedProtobufWritable<TrafficCounts>> {

    private BinaryKey cellIdAndDate;

    @Override
    protected void setup(Context context) {
        this.cellIdAndDate = new BinaryKey();
    }

    @Override
    protected void map(Text key, TypedProtobufWritable<TrafficCounts> value,
            Context context) throws IOException, InterruptedException {
        TrafficCounts counts = value.get();
        this.cellIdAndDate.setPrimaryKey(counts.getId());
        this.cellIdAndDate.setSecondaryKey(counts.getDate());
        context.write(this.cellIdAndDate, value);
    }
}
