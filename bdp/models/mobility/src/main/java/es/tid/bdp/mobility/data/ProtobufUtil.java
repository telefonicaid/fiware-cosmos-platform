package es.tid.bdp.mobility.data;

import com.google.protobuf.GeneratedMessage;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

/**
 *
 * @author dmicol
 */
public interface ProtobufUtil {
    GeneratedMessage create(Object... args);
    ProtobufWritable createAndWrap(Object... args);
}
