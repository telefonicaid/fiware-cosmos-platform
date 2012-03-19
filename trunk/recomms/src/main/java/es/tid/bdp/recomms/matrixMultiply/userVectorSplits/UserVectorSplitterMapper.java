package es.tid.bdp.recomms.matrixMultiply.userVectorSplits;

import com.google.common.io.Closeables;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import es.tid.bdp.recomms.data.RecommsProtocol.VectorOrPrefs;
import es.tid.bdp.recomms.matrixMultiply.userVectorSplits.UserVectorSplitterMapper;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FSDataInputStream;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.IntWritable;

import org.apache.mahout.cf.taste.common.TopK;
import org.apache.mahout.cf.taste.impl.common.FastIDSet;
import org.apache.mahout.common.iterator.FileLineIterable;
import org.apache.mahout.math.Vector;
import org.apache.mahout.math.VectorWritable;

import java.io.IOException;
import java.util.Comparator;
import java.util.Iterator;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public final class UserVectorSplitterMapper
        extends
        Mapper<LongWritable, VectorWritable, IntWritable, ProtobufWritable<VectorOrPrefs>> {
    private static final Logger log = LoggerFactory
            .getLogger(UserVectorSplitterMapper.class);

    static final String USERS_FILE = "usersFile";
    public static final String MAX_PREFS_PER_USER_CONSIDERED = "maxPrefsPerUserConsidered";
    static final int DEFAULT_MAX_PREFS_PER_USER_CONSIDERED = 10;
    
    private int maxPrefsPerUserConsidered;
    private FastIDSet usersToRecommendFor;

    @Override
    protected void setup(Context context) throws IOException {
        Configuration jobConf = context.getConfiguration();
        maxPrefsPerUserConsidered = jobConf.getInt(MAX_PREFS_PER_USER_CONSIDERED, DEFAULT_MAX_PREFS_PER_USER_CONSIDERED);        
        String usersFilePathString = jobConf.get(USERS_FILE);
        if (usersFilePathString != null) {
            FSDataInputStream in = null;
            try {
                Path unqualifiedUsersFilePath = new Path(usersFilePathString);
                FileSystem fs = FileSystem.get(
                        unqualifiedUsersFilePath.toUri(), jobConf);
                usersToRecommendFor = new FastIDSet();
                Path usersFilePath = unqualifiedUsersFilePath.makeQualified(fs);
                in = fs.open(usersFilePath);
                for (String line : new FileLineIterable(in)) {
                    try {
                        usersToRecommendFor.add(Long.parseLong(line));
                    } catch (NumberFormatException nfe) {
                        log.warn("usersFile line ignored: {}", line);
                    }
                }
            } finally {
                Closeables.closeQuietly(in);
            }
        }
    }

    @Override
    public void map(LongWritable key, VectorWritable value, Context context)
            throws IOException, InterruptedException {
        long userID = key.get();
        if (usersToRecommendFor != null
                && !usersToRecommendFor.contains(userID)) {
            return;
        }
        Vector userVector = maybePruneUserVector(value.get());
        Iterator<Vector.Element> it = userVector.iterateNonZero();
        IntWritable itemIndexWritable = new IntWritable();
        ProtobufWritable<VectorOrPrefs> vectorOrPref = new ProtobufWritable<VectorOrPrefs>();
        vectorOrPref.setConverter(VectorOrPrefs.class);

        while (it.hasNext()) {
            Vector.Element e = it.next();
            itemIndexWritable.set(e.index());
            vectorOrPref.set(VectorOrPrefs.newBuilder().setUserID(userID)
                    .setValue((float)e.get()).build());
            context.write(itemIndexWritable, vectorOrPref);
        }
    }
    
    private Vector maybePruneUserVector(Vector userVector) {
        if (userVector.getNumNondefaultElements() <= maxPrefsPerUserConsidered) {
          return userVector;
        }

        float smallestLargeValue = findSmallestLargeValue(userVector);

        // "Blank out" small-sized prefs to reduce the amount of partial products
        // generated later. They're not zeroed, but NaN-ed, so they come through
        // and can be used to exclude these items from prefs.
        Iterator<Vector.Element> it = userVector.iterateNonZero();
        while (it.hasNext()) {
          Vector.Element e = it.next();
          float absValue = Math.abs((float) e.get());
          if (absValue < smallestLargeValue) {
            e.set(Float.NaN);
          }
        }

        return userVector;
      }

      private float findSmallestLargeValue(Vector userVector) {

        TopK<Float> topPrefValues = new TopK<Float>(maxPrefsPerUserConsidered, new Comparator<Float>() {
          @Override
          public int compare(Float one, Float two) {
            return one.compareTo(two);
          }
        });

        Iterator<Vector.Element> it = userVector.iterateNonZero();
        while (it.hasNext()) {
          float absValue = Math.abs((float) it.next().get());
          topPrefValues.offer(absValue);
        }
        return topPrefValues.smallestGreat();
      }    

}
