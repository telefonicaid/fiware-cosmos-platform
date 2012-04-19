package es.tid.cosmos.mobility.activityarea;

import java.io.IOException;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.ActivityAreaUtil;
import es.tid.cosmos.mobility.data.RepeatedActivityAreasUtil;
import es.tid.cosmos.mobility.data.MobProtocol.ActivityArea;
import es.tid.cosmos.mobility.data.MobProtocol.RepeatedActivityAreas;

/**
 *
 * @author losa
 */
public class  IndVarsOutReducer extends Reducer<
        LongWritable,  ProtobufWritable<RepeatedActivityAreas>,
        NullWritable,  Text> {
    private static final String DELIMITER = "|";
    private static final String MISSING = "-1|-1|-1|-1|-1|-1|-1|-1";
    private static final int FIRST_MONTH = 1;
    private static final int LAST_MONTH = 6;

    @Override
    public void reduce(LongWritable key,
            Iterable<ProtobufWritable<RepeatedActivityAreas>> values,
            Context context) throws IOException, InterruptedException {
        for (ProtobufWritable<RepeatedActivityAreas> value : values) {
            value.setConverter(RepeatedActivityAreas.class);
            final RepeatedActivityAreas activityAreas = value.get();
            List<ActivityArea> areasList =
                activityAreas.getRepeatedActivityAreaList();
            for (int numMonth=FIRST_MONTH; numMonth<=LAST_MONTH; numMonth++) {
                boolean exists = false;
                String ans = key.toString();
                ans += DELIMITER + numMonth;
                for(int pos=0; pos < areasList.size(); pos++) {
                    ActivityArea area = areasList.get(pos);
                    if (area.getMonth() == numMonth && area.getIsWorkDay()) {
                        ans += DELIMITER + ActivityAreaUtil.toString(area);
                        exists = true;
                        break;
                    }
                }
                if (!exists) {
                    ans += DELIMITER + MISSING;
                }
                exists = false;
                for(int pos=0; pos < areasList.size(); pos++) {
                    ActivityArea area = areasList.get(pos);
                    if (area.getMonth() == numMonth && !area.getIsWorkDay()) {
                        ans += DELIMITER + ActivityAreaUtil.toString(area);
                        exists = true;
                        break;
                    }
                }
                if (!exists) {
                    ans += DELIMITER + MISSING;
                }
                context.write(NullWritable.get(),
                              new Text(ans));
            }
        }
    }
}
