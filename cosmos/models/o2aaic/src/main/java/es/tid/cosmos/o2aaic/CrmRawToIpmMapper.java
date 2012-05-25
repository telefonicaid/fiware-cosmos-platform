package es.tid.cosmos.o2aaic;

import java.io.IOException;
import java.security.NoSuchAlgorithmException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.util.Logger;
import es.tid.cosmos.o2aaic.data.generated.CrmProtocol.CrmIpm;
import es.tid.cosmos.o2aaic.data.generated.CrmProtocol.CrmRaw;
import es.tid.cosmos.o2aaic.util.CrmIpmUtil;
import es.tid.cosmos.o2aaic.util.CrmRawUtil;
import es.tid.cosmos.o2aaic.util.SHAEncoder;

/**
 *
 * @author dmicol
 */
public class CrmRawToIpmMapper extends Mapper<LongWritable, Text,
                                              LongWritable, Text> {
    @Override
    protected void map(LongWritable key, Text value, Context context)
            throws IOException, InterruptedException {
        final CrmRaw crmRaw = CrmRawUtil.parse(value.toString());
        String anonymisedMsisdn;
        String anonymisedBillingPostCode;
        final String anonymisedImsi;
        final String anonymisedImei;
        try {
            anonymisedMsisdn = SHAEncoder.encode(crmRaw.getMsisdn());
            anonymisedBillingPostCode = SHAEncoder.encode(
                    crmRaw.getBillingPostCode());
            anonymisedImsi = SHAEncoder.encode(crmRaw.getImsi());
            anonymisedImei = SHAEncoder.encode(crmRaw.getImei());
        } catch (NoSuchAlgorithmException ex) {
            Logger.get(CrmRawToIpmMapper.class).fatal(ex);
            throw new InterruptedException("Failed to anonimise data");
        }
        final String imeiTac = (crmRaw.getImei().length() == 15) ?
                crmRaw.getImei().substring(0, 8) : "";
        final CrmIpm crmIpm = CrmIpmUtil.create(anonymisedMsisdn,
                anonymisedBillingPostCode, "", crmRaw.getAcornCode(),
                crmRaw.getGender(), crmRaw.getBillingSystem(),
                crmRaw.getMtrcPlSegment(), crmRaw.getMpnStatus(),
                crmRaw.getSpid(), crmRaw.getActiveStatus(),
                crmRaw.getNeedsSegmentation(), crmRaw.getAge(),
                crmRaw.getAgeBand(), anonymisedImsi, anonymisedImei, imeiTac,
                crmRaw.getDeviceType(), crmRaw.getDeviceManufacturer(),
                crmRaw.getDeviceModelName(), crmRaw.getEffectiveFromDate());
        context.write(key, new Text(CrmIpmUtil.toString(crmIpm)));
    }
}
