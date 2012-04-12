package es.tid.cosmos.mobility.mapreduce;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.MobProtocol.Cdr;
import es.tid.cosmos.mobility.data.MobProtocol.Cell;
import es.tid.cosmos.mobility.parsing.CellParser;

/**
 *
 * @author sortega
 */
public class ParseCellsMapper extends Mapper<LongWritable, Text, LongWritable,
        ProtobufWritable<Cdr>> {
    private LongWritable cellId;
    
    @Override
    public void setup(Context context) {
        this.cellId = new LongWritable();
    }
    
    @Override
    public void map(LongWritable lineno, Text line, Context context)
            throws IOException, InterruptedException {
        final Cell cell = new CellParser(line.toString()).parse();
        ProtobufWritable wrappedCdr = ProtobufWritable.newInstance(Cell.class);
        wrappedCdr.set(cell);

        this.cellId.set(cell.getCellId());
        context.write(this.cellId, wrappedCdr);
    }
}
