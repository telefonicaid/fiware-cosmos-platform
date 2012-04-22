package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

/**
 *
 * @author dmicol
 */
public interface ProtobufUtil {
    ProtobufWritable wrap(ProtobufWritable obj);
}
