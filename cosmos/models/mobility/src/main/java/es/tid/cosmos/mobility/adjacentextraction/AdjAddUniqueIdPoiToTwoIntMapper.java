package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Counter;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, Poi>
 * Output: <Long, TwoInt>
 * 
 * @author dmicol
 */
class AdjAddUniqueIdPoiToTwoIntMapper extends Mapper<ProtobufWritable<TwoInt>,
        TypedProtobufWritable<Poi>, LongWritable, TypedProtobufWritable<TwoInt>> {
    
    private Counter counter;
    
    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        this.counter = context.getCounter(Counters.COUNTER_FOR_TABLE_ID);
    }
    
    @Override
    protected void map(ProtobufWritable<TwoInt> key,
            TypedProtobufWritable<Poi> value, Context context)
            throws IOException, InterruptedException {
        final long tableId = this.counter.getValue();
        this.counter.increment(1L);
        final TwoInt poiPoimod = TwoIntUtil.create(tableId, tableId);
        context.write(new LongWritable(tableId),
                      new TypedProtobufWritable<TwoInt>(poiPoimod));
    }
}
