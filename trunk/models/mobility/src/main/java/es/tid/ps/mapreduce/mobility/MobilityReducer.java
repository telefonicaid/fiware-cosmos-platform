package es.tid.ps.mapreduce.mobility;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.URI;
import java.text.ParseException;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.filecache.DistributedCache;
import org.apache.hadoop.fs.FSDataInputStream;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;
import org.quartz.CronExpression;

import es.tid.ps.mapreduce.mobility.data.CdrUserData;
import es.tid.ps.mapreduce.mobility.data.CellData;
import es.tid.ps.mapreduce.mobility.data.CompositeKey;
import es.tid.ps.mapreduce.mobility.data.FileType;
import es.tid.ps.mapreduce.mobility.data.MobilityCounter;

/**
 * This class makes the reduce phase in the generation of mobility in realise
 * 1.0. The partitioner of the process is override so, at each map arrives the
 * data of each user sorted by type, this means that firsts arrives the input
 * from users into the platform and then arrives the data of the CDRs. With this
 * arrangement we filter the users that are into the platform checking if the
 * user identifier corresponds with data last input about User Information.
 * 
 * The we used CronExpression to validate if the date is into some times in
 * where we count the data, if this is true we increment a counter.
 * 
 * When all data of the user have been processed we review what is the maximum
 * in each time and emit a pair key/value
 * 
 * TODO rgc: now we have the timezones defines and codifies directly into the
 * code, and the best options will be out to configuration
 * 
 * @author rgc
 */
public class MobilityReducer extends

Reducer<CompositeKey, Text, Text, LongWritable> {
    String actualUser = "";
    private Map<String, Long> workCalls;
    private Map<String, Long> homeCalls;
    private CronExpression homeDateValidator;
    private CronExpression workDateValidator;
    private Map<String, CellData> cacheCells;

    /**
     * @param key
     *            is the key of the mapper. The key has the naturalKey (user
     *            identifier) and the secondary key (file type)
     * @param values
     *            are all the values aggregated during the mapping phase
     * @param context
     *            contains the context of the job run
     */
    @Override
    protected void reduce(CompositeKey key, Iterable<Text> values,
            Context context) throws IOException, InterruptedException {
        // check if the user is into the platform (Is into the fileTypes 0)
        if (FileType.FILE_USERS.getValue().equals(key.getFileIdKey())) {
            updateUserInProcess(key.getUserIdKey());
            return;
        }
        if (!actualUser.equals(key.getUserIdKey())) {
            context.getCounter(MobilityCounter.USER_NOT_IN_PLATFORM)
                    .increment(1L);
            return;
        }
        for (Text line : values) {
            processLine(line.toString(), context);
        }
        writeResults(workCalls, "work", context);
        writeResults(homeCalls, "home", context);
    }

    /**
     * Method that process a cdrs line
     * 
     * @param line
     *            string with the data
     * @param context
     *            contains the context of the job run
     */
    private void processLine(String line, Context context) {
        try {
            CdrUserData cdrUserData = CdrUserData.parse(line);
            updateCallByUserAndCell(cdrUserData.getInitCellId(),
                    cdrUserData.getDateCall());
            updateCallByUserAndCell(cdrUserData.getEndCellId(),
                    cdrUserData.getDateCall());
        } catch (Exception e) {
            context.getCounter(MobilityCounter.LINE_PARSER_CDRS_ERROR)
                    .increment(1L);
        }
    }

    /**
     * Method that reads the configuration and prepair the date validators.
     * 
     * @param context
     *            contains the context of the job run
     */
    @Override
    protected void setup(Context context) throws IOException,
            InterruptedException {
        try {
            Configuration conf = context.getConfiguration();
            this.homeDateValidator = new CronExpression(conf.get(
                    "models.mobility.home.cron", "* * 8-15 ? * 1-4"));
            this.workDateValidator = new CronExpression(conf.get(
                    "models.mobility.work.cron", "* * 8-15 ? * 1-4"));

            this.cacheCells = new HashMap<String, CellData>();

            URI[] pathCaches = DistributedCache.getCacheFiles(context
                    .getConfiguration());
            if (pathCaches != null) {
                for (int i = 0; i < pathCaches.length; i++) {
                    loadCacheCells(new Path(pathCaches[i]), conf);
                }
            }
        } catch (ParseException e) {
            // TODO rgc write log for error 
            throw new RuntimeException(e);
        }
    }

    /**
     * Method that loads the cache with the data of the cells
     * 
     * @param path
     *            A path array of localized caches
     * @param conf
     *            Configuration that contains the localized archives
     * @throws IOException
     */
    public void loadCacheCells(Path path, Configuration conf)
            throws IOException {
        FSDataInputStream in = FileSystem.get(conf).open(path);
        BufferedReader br = new BufferedReader(new InputStreamReader(in));
        String line;
        try {
            while ((line = br.readLine()) != null) {
                CellData cellData = CellData.parse(line);
                this.cacheCells.put(cellData.getCellId(), cellData);
            }
        } finally {
            br.close();
        }
    }

    /**
     * Method that checks the maximum value for each time and emit the
     * correspondent pair of key/value
     * 
     * @param map
     *            map structure that we want to check
     * @param context
     *            contains the context of the job run
     * @param description
     *            description of timezone
     * @param description
     *            description of timezone
     * @throws IOException
     * @throws InterruptedException
     */
    private void writeResults(Map<String, Long> map, String description,
            Context context) throws IOException, InterruptedException {
        List<Entry<String, Long>> results = getResults(map);
        for (Entry<String, Long> result : results) {
            if (result != null) {
                // TODO define the correct output keys, now contact the key
                // ouput (is is horrible this implementation but it is
                // funtional)
                context.write(new Text(this.actualUser + " " + description
                        + " " + result.getKey()),
                        new LongWritable(result.getValue()));
            }
        }
    }

    /**
     * Method that updates each timezones in where this is include the cdr data
     * 
     * @param cellId
     *            identifier of the cell
     * @param timeCall
     *            date of the call
     */
    private void updateCallByUserAndCell(String cellId, Date timeCall) {
        if (this.workDateValidator.isSatisfiedBy(timeCall)) {
            updateCallCell(this.workCalls, cellId, 1L);
        }
        if (this.homeDateValidator.isSatisfiedBy(timeCall)) {
            updateCallCell(this.homeCalls, cellId, 1L);
        }
    }

    /**
     * Method that prepares the structures for each user
     * 
     * @param newUser
     *            user identifier
     */
    public void updateUserInProcess(String newUser) {
        this.actualUser = newUser;
        this.workCalls = new HashMap<String, Long>();
        this.homeCalls = new HashMap<String, Long>();
    }

    /**
     * Static method that increments the entry value or create a new entry with
     * the value that recives by param, if there are no values into the map that
     * recives by param
     * 
     * @param map
     *            map structure that we want to update
     * @param key
     *            identifier the entry
     */
    private static void updateCallCell(Map<String, Long> map, String key,
            Long inc) {
        Long value = map.get(key);
        if (value == null) {
            map.put(key, new Long(inc));
        } else {
            map.put(key, value + inc);
        }
    }

    /**
     * Method that calculate the response, this method process the bts, states
     * and countries from the data of the cell
     * 
     * @param cellMap
     *            a map with the results of the cell by user
     * @return arraylist with the identifier of the cell, bts, state and country
     *         in the first value, and the number of calls in this element in
     *         the second value
     */
    private List<Entry<String, Long>> getResults(Map<String, Long> cellMap) {
        Map<String, Long> btsMap = new HashMap<String, Long>();
        Map<String, Long> statesMap = new HashMap<String, Long>();
        Map<String, Long> countryMap = new HashMap<String, Long>();
        for (Map.Entry<String, Long> entry : cellMap.entrySet()) {
            Long value = entry.getValue();
            CellData cellData = this.cacheCells.get(entry.getKey());
            updateCallCell(btsMap, cellData.getBts(), value);
            updateCallCell(statesMap, cellData.getState(), value);
            updateCallCell(countryMap, cellData.getCountry(), value);
        }
        List<Entry<String, Long>> results = new ArrayList<Map.Entry<String, Long>>(
                4);
        results.add(getMaxValue(cellMap));
        results.add(getMaxValue(btsMap));
        results.add(getMaxValue(statesMap));
        results.add(getMaxValue(countryMap));
        return results;
    }

    /**
     * Static method that returns entre thata has the maximum value into a hash
     * map.
     * 
     * @param map
     *            map structure that we want to check
     * @return the entry <String, Long> the the maximum Long value
     */
    private static Entry<String, Long> getMaxValue(Map<String, Long> map) {
        Map.Entry<String, Long> maxEntry = null;
        for (Map.Entry<String, Long> entry : map.entrySet()) {
            if (maxEntry == null
                    || entry.getValue().compareTo(maxEntry.getValue()) > 0) {
                maxEntry = entry;
            }
        }
        return maxEntry;
    }
}
