package es.tid.cosmos.mobility.parsing;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.util.Logger;
import es.tid.cosmos.mobility.conf.MobilityConfiguration;
import es.tid.cosmos.mobility.data.DateUtil;
import es.tid.cosmos.mobility.data.generated.BaseProtocol;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cdr;

/**
 * Parses and filters by date the input CDRs.
 *
 * Input: <Long, Text>
 * Output: <Long, Cdr>
 * 
 * @author dmicol, sortega
 */
public class ParseCdrMapper extends Mapper<LongWritable, Text, LongWritable,
        TypedProtobufWritable<Cdr>> {

    private String separator;
    private DateParser dateParser;
    private BaseProtocol.Date startDate;
    private BaseProtocol.Date endDate;

    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        final MobilityConfiguration conf = new MobilityConfiguration(
                context.getConfiguration());
        this.separator = conf.getDataSeparator();
        this.dateParser = new DateParser(conf.getDataDateFormat());
        this.startDate = parseOptionalDate(conf.getDataStartDate());
        this.endDate = parseOptionalDate(conf.getDataEndDate());
    }

    private BaseProtocol.Date parseOptionalDate(String input) {
        if (input.trim().isEmpty()) {
            return null;
        }
        return this.dateParser.parse(input);
    }

    @Override
    public void map(LongWritable key, Text value, Context context)
            throws IOException, InterruptedException {
        final String line = value.toString();
        try {
            final Cdr cdr = new CdrParser(line, this.separator,
                                          this.dateParser).parse();
            context.getCounter(Counters.VALID_RECORDS).increment(1L);
            if (this.isOnRange(cdr.getDate())) {
                context.getCounter(Counters.SELECTED_RECORDS).increment(1L);
                context.write(new LongWritable(cdr.getUserId()),
                              new TypedProtobufWritable<Cdr>(cdr));
            }
        } catch (Exception ex) {
            Logger.get(ParseCdrMapper.class).warn("Invalid line: " + line);
            context.getCounter(Counters.INVALID_RECORDS).increment(1L);
        }
    }

    private boolean isOnRange(BaseProtocol.Date date) {
        return (this.startDate == null ||
                DateUtil.compare(date, this.startDate) >= 0) &&
               (this.endDate == null ||
                DateUtil.compare(date, this.endDate) <= 0);
    }
}
