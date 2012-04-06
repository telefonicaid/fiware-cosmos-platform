package es.tid.bdp.mobility.mapreduce;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.bdp.mobility.data.MobProtocol.Cdr;
import es.tid.bdp.mobility.data.MobProtocol.Cell;
import es.tid.bdp.mobility.parsing.CellParser;

/**
 *
 * @author sortega
 */
public class ParseCellsMapper extends Mapper<IntWritable, Text, LongWritable,
        ProtobufWritable<Cdr>> {
    private LongWritable cellId;
    
    @Override
    protected void setup(Context context) {
        this.cellId = new LongWritable();
    }
    
    @Override
    protected void map(IntWritable lineno, Text line, Context context)
            throws IOException, InterruptedException {
        final Cell cell = new CellParser(line.toString()).parse();
        ProtobufWritable wrappedCdr = ProtobufWritable.newInstance(Cell.class);
        wrappedCdr.set(cell);
        this.cellId.set(cell.getCellId());
        context.write(this.cellId, wrappedCdr);
    }
}
