package es.tid.cosmos.mobility.mivs;

import java.io.IOException;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.MobVars;
import es.tid.cosmos.mobility.data.MobProtocol.MobViMobVars;
import es.tid.cosmos.mobility.data.MobVarsUtil;

/**
 * Input: <Long, MobViMobVars>
 * Output: <Null, Text>
 * 
 * @author logc
 */
public class  IndVarsOutReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, NullWritable, Text> {
    private static final String DELIMITER = "|";
    private static final String MISSING = "-1|-1|-1|-1|-1|-1|-1|-1";
    private static final int FIRST_MONTH = 1;
    private static final int LAST_MONTH = 6;

    @Override
    public void reduce(LongWritable key,
            Iterable<ProtobufWritable<MobData>> values,
            Context context) throws IOException, InterruptedException {
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobViMobVars activityAreas = value.get().getMobViMobVars();
            List<MobVars> areasList = activityAreas.getVarsList();
            for (int numMonth = FIRST_MONTH; numMonth <= LAST_MONTH; numMonth++) {
                boolean exists = false;
                String ans = key.toString();
                ans += DELIMITER + numMonth;
                for (MobVars area : areasList) {
                    if (area.getMonth() == numMonth && area.getWorkingday()) {
                        ans += DELIMITER + MobVarsUtil.toString(area);
                        exists = true;
                        break;
                    }
                }
                if (!exists) {
                    ans += DELIMITER + MISSING;
                }
                exists = false;
                for (MobVars area : areasList) {
                    if (area.getMonth() == numMonth && !area.getWorkingday()) {
                        ans += DELIMITER + MobVarsUtil.toString(area);
                        exists = true;
                        break;
                    }
                }
                if (!exists) {
                    ans += DELIMITER + MISSING;
                }
                context.write(NullWritable.get(), new Text(ans));
            }
        }
    }
}
