/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

package es.tid.cosmos.profile.export.text;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.io.NullWritable;

import es.tid.cosmos.profile.data.UserProfileUtil;
import es.tid.cosmos.profile.generated.data.ProfileProtocol.UserProfile;

/**
 *
 * @author dmicol
 */
public class TextExporterReducer extends Reducer<Text,
        ProtobufWritable<UserProfile>, NullWritable, Text> {
    @Override
    public void reduce(Text key,
            Iterable<ProtobufWritable<UserProfile>> values, Context context)
            throws IOException, InterruptedException {
        for (ProtobufWritable<UserProfile> value : values) {
            value.setConverter(UserProfile.class);
            context.write(NullWritable.get(),
                          new Text(UserProfileUtil.toString(value.get())));
        }
    }
}
