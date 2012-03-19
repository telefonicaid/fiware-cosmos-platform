package es.tid.bdp.recomms.common;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.mahout.common.Pair;
import org.apache.mahout.common.iterator.sequencefile.PathFilters;
import org.apache.mahout.common.iterator.sequencefile.PathType;
import org.apache.mahout.common.iterator.sequencefile.SequenceFileDirIterable;
import org.apache.mahout.math.Vector;
import org.apache.mahout.math.map.OpenIntLongHashMap;

public class TIDUtils {
    public static class TemporaryElement implements Vector.Element {

        private final int index;
        private double value;

        TemporaryElement(int index, double value) {
            this.index = index;
            this.value = value;
        }

        public TemporaryElement(Vector.Element toClone) {
            this(toClone.index(), toClone.get());
        }

        @Override
        public double get() {
            return value;
        }

        @Override
        public int index() {
            return index;
        }

        @Override
        public void set(double value) {
            this.value = value;
        }
    }
    public static OpenIntLongHashMap readItemIDIndexMap(String itemIDIndexPathStr, Configuration conf) {
        OpenIntLongHashMap indexItemIDMap = new OpenIntLongHashMap();
        Path itemIDIndexPath = new Path(itemIDIndexPathStr);
        for (Pair<IntWritable,LongWritable> record
             : new SequenceFileDirIterable<IntWritable,LongWritable>(itemIDIndexPath,
                                                                           PathType.LIST,
                                                                           PathFilters.partFilter(),
                                                                           null,
                                                                           true,
                                                                           conf)) {
          indexItemIDMap.put(record.getFirst().get(), record.getSecond().get());
        }
        return indexItemIDMap;
      }
}
