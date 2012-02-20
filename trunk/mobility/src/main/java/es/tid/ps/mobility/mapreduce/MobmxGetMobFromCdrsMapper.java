package es.tid.ps.mobility.mapreduce;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.ps.mobility.data.BaseProtocol.Date;
import es.tid.ps.mobility.data.GassetProtocol.GstCdr;
import es.tid.ps.mobility.data.MxCdrUtil;
import es.tid.ps.mobility.data.MxProtocol.MxCdr;

/**
 *
 * @author dmicol
 */
public class MobmxGetMobFromCdrsMapper extends Mapper<IntWritable,
        ProtobufWritable<GstCdr>, IntWritable, ProtobufWritable<MxCdr>> {
    @Override
    public void map(IntWritable key, ProtobufWritable<GstCdr> value,
            Context context) throws IOException, InterruptedException {
        GstCdr gstCdr = value.get();

        Date date = gstCdr.getDate();
        Date.Builder dateBuilder = date.toBuilder();
        dateBuilder.setWeekDay((date.getWeekDay() + 1) % 7);

        ProtobufWritable<MxCdr> mxCdr = MxCdrUtil.createAndWrap(key.get(),
                gstCdr.getCellId(), date, gstCdr.getTime());
        context.write(key, mxCdr);
    }
}
