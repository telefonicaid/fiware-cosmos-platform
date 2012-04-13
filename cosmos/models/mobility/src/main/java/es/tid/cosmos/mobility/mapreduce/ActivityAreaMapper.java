package es.tid.cosmos.mobility.mapreduce;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.ActivityAreaUtil;
import es.tid.cosmos.mobility.data.MobProtocol.ActivityArea;
import es.tid.cosmos.mobility.data.MobProtocol.ActivityAreaKey;
import es.tid.cosmos.mobility.data.MobProtocol.Cell;

/**
 *
 * @author losa
 */
public class ActivityAreaMapper extends Mapper<
            ProtobufWritable<ActivityAreaKey>, ProtobufWritable<Cell>,
            ProtobufWritable<ActivityAreaKey>, ProtobufWritable<Cell>> {
    /*
     * the default map is the identity function
     */
}
