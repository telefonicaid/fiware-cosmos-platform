package es.tid.bdp.mobility.mapreduce;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.bdp.mobility.data.BaseProtocol.Date;
import es.tid.bdp.mobility.data.CdrUtil;
import es.tid.bdp.mobility.data.GassetProtocol.GstCdr;
import es.tid.bdp.mobility.data.MobProtocol.Cdr;

/**
 *
 * @author dmicol
 */
public class GetMobFromCdrsMapper extends Mapper<IntWritable,
        ProtobufWritable<GstCdr>, IntWritable, ProtobufWritable<Cdr>> {
    @Override
    public void map(IntWritable key, ProtobufWritable<GstCdr> value,
            Context context) throws IOException, InterruptedException {
        final GstCdr gstCdr = value.get();
        final Date date = gstCdr.getDate();
        Date.Builder dateBuilder = date.toBuilder();
        dateBuilder.setWeekday((date.getWeekday() + 1) % 7);
        Date modifiedDate = dateBuilder.build();

        ProtobufWritable<Cdr> mxCdr = CdrUtil.createAndWrap(key.get(),
                gstCdr.getCellId(), modifiedDate, gstCdr.getTime());
        context.write(key, mxCdr);
    }
}
