package es.tid.ps.mobility.mapreduce;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.ps.mobility.data.MobProtocol.Cdr;
import es.tid.ps.mobility.data.MobProtocol.Cell;
import es.tid.ps.mobility.parsing.CellParser;

/**
 *
 * @author sortega
 */
public class ParseCellsMapper extends Mapper<IntWritable, Text, LongWritable,
        ProtobufWritable<Cdr>> {

    @Override
    protected void map(IntWritable lineno, Text line, Context context)
            throws IOException, InterruptedException {
        final Cell cell = new CellParser(line.toString()).parse();
        ProtobufWritable wrappedCdr = ProtobufWritable.newInstance(Cell.class);
        wrappedCdr.set(cell);
        context.write(new LongWritable(cell.getCellId()), wrappedCdr);
    }
}
