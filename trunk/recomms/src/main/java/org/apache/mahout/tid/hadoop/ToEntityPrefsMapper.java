/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.apache.mahout.tid.hadoop;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.mahout.cf.taste.hadoop.item.RecommenderJob;
import org.apache.mahout.cf.taste.hadoop.EntityPrefWritable;
import org.apache.mahout.math.VarLongWritable;


import java.io.IOException;

public abstract class ToEntityPrefsMapper
        extends
        Mapper<VarLongWritable, EventsWritable, VarLongWritable, VarLongWritable> {

    public static final String TRANSPOSE_USER_ITEM = ToEntityPrefsMapper.class
            + "transposeUserItem";
    public static final String RATING_SHIFT = ToEntityPrefsMapper.class
            + "shiftRatings";

    private boolean booleanData;
    private boolean transpose;
    private final boolean itemKey;

    ToEntityPrefsMapper(boolean itemKey) {
        this.itemKey = itemKey;
    }

    @Override
    protected void setup(Context context) {
        Configuration jobConf = context.getConfiguration();
        booleanData = jobConf.getBoolean(RecommenderJob.BOOLEAN_DATA, false);
        transpose = jobConf.getBoolean(TRANSPOSE_USER_ITEM, false);
    }

    @Override
    public void map(VarLongWritable key, EventsWritable value, Context context)
            throws IOException, InterruptedException {
        long userID = Long.parseLong(key.toString());
        long itemID = value.getID();
        if (itemKey ^ transpose) {
            // If using items as keys, and not transposing items and users, then
            // users are items!
            // Or if not using items as keys (users are, as usual), but
            // transposing items and users,
            // then users are items! Confused?
            long temp = userID;
            userID = itemID;
            itemID = temp;
        }
        if (booleanData) {
            context.write(new VarLongWritable(userID), new VarLongWritable(
                    itemID));
        } else {
            context.write(new VarLongWritable(userID), new EntityPrefWritable(itemID, value.getEventValue()));
        }
    }

}
