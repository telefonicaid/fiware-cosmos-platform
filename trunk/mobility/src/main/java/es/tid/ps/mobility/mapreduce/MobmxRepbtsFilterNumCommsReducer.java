/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package es.tid.ps.mobility.mapreduce;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import java.io.IOException;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.ps.mobility.data.MxProtocol.MxCdr;
import es.tid.ps.mobility.data.MxProtocol.NodeBtsDay;
import java.util.ArrayList;
import java.util.List;

/**
 *
 * @author dmicol
 */
public class MobmxRepbtsFilterNumCommsReducer extends Reducer<IntWritable,
        ProtobufWritable<NodeBtsDay>, IntWritable, ProtobufWritable<MxCdr>> {
    @Override
    protected void reduce(IntWritable key,
            Iterable<ProtobufWritable<NodeBtsDay>> values, Context context)
            throws IOException, InterruptedException {
        /*List<NodeBtsDay> nodes = new ArrayList<NodeBtsDay>();
        for (ProtobufWritable<NodeBtsDay> node : values) {
            nodes.add(node.get());
        }

        // Inputs
        MRData_UInt node;
        MRData_Node_Bts_Day input;
        // Outputs
        MRData_UInt ncomms;

        int numCommsInfo;
        int numCommsNoinfo;
        int numCommsNobts;
        numCommsInfo = numCommsNoinfo = 0;

        if (nodes.get(0).num_kvs == 0) {
            return;
        }
        node.parse(nodes.get(0).kvs[0].key);

        // Num of communications with bts info
        for (int i = 0; i < nodes.get(0).num_kvs; i++) {
            input.parse(nodes.get(0).kvs[i].value);
            numCommsInfo += input.count;
        }

        // Num of communications without bts info
        numCommsNoinfo = nodes.get(1).num_kvs;
        numCommsNobts = nodes.get(2).num_kvs;

        // Filter by total num of communications
        if ((numCommsInfo + numCommsNoinfo + numCommsNobts) <
                mob_conf_min_number_total_calls) {
            ncomms.value = numCommsInfo + numCommsNoinfo + numCommsNobts;
        } else if ((numCommsInfo + numCommsNoinfo + numCommsNobts) >
                mob_conf_max_number_total_calls) {
            ncomms.value = numCommsInfo + numCommsNoinfo + numCommsNobts;
        } else {
            ncomms.value = numCommsInfo;
            context.write(node, ncomms);
        }*/
    }
}
