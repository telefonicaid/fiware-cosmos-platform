package es.tid.bdp.profile.export.ps;

import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Counter;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;

import es.tid.bdp.profile.dictionary.Dictionary;
import es.tid.bdp.profile.dictionary.comscore.DistributedCacheDictionary;
import es.tid.bdp.profile.generated.data.ProfileProtocol.CategoryCount;
import es.tid.bdp.profile.generated.data.ProfileProtocol.UserProfile;

/**
 *
 * @author dmicol, sortega
 */
public class PSExporterReducer extends Reducer<Text,
                                               ProtobufWritable<UserProfile>,
                                               NullWritable, Text> {
    public static final String PSEXPORT_SERVICE = "psexport.service";
    public static final String PSEXPORT_USER = "psexport.user";
    public static final String PSEXPORT_SOURCE = "psexport.source";
    public static final String PSEXPORT_TIMESTAMP = "psexport.timestamp";

    private static final String FILENAME_FORMAT = "/psprofile_%s_%s.dat";
    private static final SimpleDateFormat RECORD_TIMESTAMP_FORMAT =
            new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss'Z'");
    private static final SimpleDateFormat FILENAME_TIMESTAMP_FORMAT =
            new SimpleDateFormat("yyyyMMdd-HHmmss");
    private static final String DEFAULT_PSEXPORT_USER = "PS";
    private static final String DEFAULT_PSEXPORT_SERVICE = "kpi";
    private static final String DEFAULT_PSEXPORT_SOURCE = "BDP";
    private static Dictionary sharedDictionary = null;
    private static String[] sharedCategoryNames = null;

    private StringBuilder builder;
    private Text record;
    private Map<String, CategoryCount> categories;
    private Counter recordCounter;
    private String timestamp;
    private String source;

    /**
     * Sets at configuration level the export timestampo for al reducers.
     *
     * @param configuration
     * @param date           Timestamp
     */
    public static void setTimestamp(Job job, Date date) {
        job.getConfiguration().setLong(PSEXPORT_TIMESTAMP, date.getTime());
    }

    public static Path getOutputFileName(Job job) {
        Path outputPath = FileOutputFormat.getOutputPath(job);
        Configuration config = job.getConfiguration();
        String timestamp = FILENAME_TIMESTAMP_FORMAT.format(getTimestamp(config));
        return outputPath.suffix(String.format(FILENAME_FORMAT,
                config.get(PSEXPORT_SOURCE, DEFAULT_PSEXPORT_SOURCE), timestamp));
    }

    @Override
    public void setup(Context context) throws IOException {
        this.setupDictionary(context);

        this.builder = new StringBuilder();
        this.record = new Text();
        this.categories = new HashMap<String, CategoryCount>();
        this.recordCounter = context.getCounter(PSExporterCounter.NUM_RECORDS);
        this.timestamp = RECORD_TIMESTAMP_FORMAT.format(
                getTimestamp(context.getConfiguration()));
        this.source = context.getConfiguration().get(PSEXPORT_SOURCE,
                                                     DEFAULT_PSEXPORT_SOURCE);
    }

    protected void setupDictionary(Context context) throws IOException {
        if (sharedDictionary == null) {
            sharedDictionary = DistributedCacheDictionary
                    .loadFromCache(context.getConfiguration());
            setCategoryNames(sharedDictionary.getAllCategoryNames());
        }
    }

    protected static void setCategoryNames(String[] categoryNames) {
        sharedCategoryNames = categoryNames.clone();
    }

    @Override
    public void reduce(Text userId,
                       Iterable<ProtobufWritable<UserProfile>> profiles,
                       Context context) throws IOException,
                                               InterruptedException {
        for (ProtobufWritable<UserProfile> wrappedProfile : profiles) {
            wrappedProfile.setConverter(UserProfile.class);
            UserProfile profile = wrappedProfile.get();
            if (isFirstRecord()) {
                writeHeaders(context);
            }
            this.categories.clear();
            for (CategoryCount count : profile.getCountsList()) {
                this.categories.put(count.getName(), count);
            }
            writeDataRecord(userId, profile, context);
        }
    }

    @Override
    public void cleanup(Context context) throws IOException,
                                                InterruptedException {
        newRecord();
        this.builder.append("F|")
                .append(this.recordCounter.getValue());
        this.record.set(this.builder.toString());
        context.write(NullWritable.get(), this.record);
    }

    private void writeDataRecord(Text userId, UserProfile profile,
                                 Context context) throws InterruptedException,
                                                         IOException {
        Configuration config = context.getConfiguration();
        newRecord();
        this.builder.append("I|")
                    .append(config.get(PSEXPORT_SERVICE,
                                       DEFAULT_PSEXPORT_SERVICE))
                    .append("|")
                    .append(userId)
                    .append("_")
                    // TODO: we shouldn't be storing tabbed dates in kpi
                    //       calculation
                    .append(profile.getDate().replaceAll("\t", ""))
                    .append("|")
                    .append(this.timestamp)
                    .append("|")
                    .append(this.source);
        for (String categoryName : sharedCategoryNames) {
            this.builder.append("|");
            long value = 0L;
            if (this.categories.containsKey(categoryName)) {
                CategoryCount count =
                        (CategoryCount)this.categories.get(categoryName);
                value = count.getCount();
            }
            this.builder.append(value);
        }
        writeRecord(context);
        this.recordCounter.increment(1L);
    }

    private void writeHeaders(Context context) throws IOException,
                                                      InterruptedException {
        Configuration config = context.getConfiguration();
        newRecord();
        this.builder.append("M|")
                    .append(config.get(PSEXPORT_USER, DEFAULT_PSEXPORT_USER))
                    .append("|")
                    .append("unnamed") // TODO: USE CSV FILENAME
                    .append("||")
                    .append(this.timestamp);
        writeRecord(context);

        newRecord();
        this.builder.append("H|")
                    .append(config.get(PSEXPORT_SERVICE,
                                       DEFAULT_PSEXPORT_SERVICE))
                    .append("|service_user_id|update_date|update_source");
        for (String categoryName : sharedCategoryNames) {
            this.builder.append("|")
                        .append(categoryName);
        }
        writeRecord(context);
    }

    private void newRecord() {
        this.builder.setLength(0);
    }

    private boolean isFirstRecord() {
        return this.recordCounter.getValue() == 0L;
    }

    private void writeRecord(Context context) throws IOException,
                                                     InterruptedException {
        this.record.set(this.builder.toString());
        context.write(NullWritable.get(), this.record);
    }

    private static Date getTimestamp(Configuration config) {
        long timestamp = config.getLong(PSEXPORT_TIMESTAMP, 0);
        if (timestamp == 0) {
            throw new IllegalStateException("Undefined timestamp");
        } else {
            return new Date(timestamp);
        }
    }
}
