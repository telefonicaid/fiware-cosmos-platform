package es.tid.cosmos.mobility.parsing;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.CellUtil;
import es.tid.cosmos.mobility.data.MobProtocol.Cell;

/**
 *
 * @author sortega
 */
public class ParseCellsReducer extends Reducer<LongWritable, Text, LongWritable,
        ProtobufWritable<Cell>> {
    private LongWritable cellId;
    
    @Override
    public void setup(Context context) {
        this.cellId = new LongWritable();
    }
    
    @Override
    public void reduce(LongWritable key, Iterable<Text> values, Context context)
            throws IOException, InterruptedException {
        for (Text value : values) {
            ProtobufWritable<Cell> cell =
                    CellUtil.wrap(new CellParser(value.toString()).parse());
            this.cellId.set(cell.get().getCellId());
            context.write(this.cellId, cell);
        }
    }
}
