package es.tid.cosmos.mobility.data;

import com.google.protobuf.GeneratedMessage;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

/**
 *
 * @author dmicol
 */
public interface ProtobufUtil {
    GeneratedMessage create(Object...args);
    ProtobufWritable wrap(Object obj);
    ProtobufWritable createAndWrap(Object...args);
}
