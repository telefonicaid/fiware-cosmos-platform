package es.tid.cosmos.o2aaic;

import java.io.IOException;
import java.security.NoSuchAlgorithmException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.util.Logger;
import es.tid.cosmos.o2aaic.data.generated.InetProtocol.InetIpm;
import es.tid.cosmos.o2aaic.data.generated.InetProtocol.InetRaw;
import es.tid.cosmos.o2aaic.util.InetIpmUtil;
import es.tid.cosmos.o2aaic.util.InetRawUtil;
import es.tid.cosmos.o2aaic.util.SHAEncoder;

/**
 *
 * @author dmicol
 */
public class InetRawToIpmMapper extends Mapper<LongWritable, Text,
                                               LongWritable, Text> {
    @Override
    protected void map(LongWritable key, Text value, Context context)
            throws IOException, InterruptedException {
        final InetRaw inetRaw = InetRawUtil.parse(value.toString());
        final String anonymisedImsi;
        final String anonymisedImei;
        try {
            anonymisedImsi = SHAEncoder.encode(inetRaw.getImsi());
            anonymisedImei = SHAEncoder.encode(inetRaw.getImei());
        } catch (NoSuchAlgorithmException ex) {
            Logger.get(InetRawToIpmMapper.class).fatal(ex);
            throw new InterruptedException("Failed to anonimise data");
        }
        final String imeiTac = (inetRaw.getImei().length() == 15) ?
                inetRaw.getImei().substring(0, 8) : "";
        final InetIpm inetIpm = InetIpmUtil.create(inetRaw.getType(),
                inetRaw.getCallType(), anonymisedImsi,
                inetRaw.getFirstTempImsi(), inetRaw.getLastTempImsi(),
                imeiTac, anonymisedImei, inetRaw.getLacod(),
                inetRaw.getCellId(), inetRaw.getEventDateTime(),
                inetRaw.getDtapCause(), inetRaw.getBssmapCause(),
                inetRaw.getCcCause(), inetRaw.getMmCause(),
                inetRaw.getRanapCause());
        context.write(key, new Text(InetIpmUtil.toString(inetIpm)));
    }
}
