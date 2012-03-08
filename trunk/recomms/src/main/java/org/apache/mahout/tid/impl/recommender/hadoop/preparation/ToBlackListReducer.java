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

package org.apache.mahout.tid.impl.recommender.hadoop.preparation;

import java.io.IOException;

import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import org.apache.mahout.math.RandomAccessSparseVector;

import org.apache.mahout.math.VarLongWritable;
import org.apache.mahout.math.Vector;
import org.apache.mahout.math.VectorWritable;
import org.apache.mahout.cf.taste.hadoop.TasteHadoopUtils;
import org.bson.BSONObject;

import com.mongodb.BasicDBObject;

public final class ToBlackListReducer extends
        Reducer<VarLongWritable, VarLongWritable, Text, BSONObject> {

    public BasicDBObject toObject(VectorWritable id) {
        BasicDBObject json = new BasicDBObject();
        json.put("value", id.toString());
        return json;
    }

    @Override
    public void reduce(VarLongWritable index,
            Iterable<VarLongWritable> possibleItemIDs, Context context)
            throws IOException, InterruptedException {
        Vector userVector = new RandomAccessSparseVector(Integer.MAX_VALUE, 100);
        for (VarLongWritable item : possibleItemIDs) {
            int item_index = TasteHadoopUtils.idToIndex(item.get());
            userVector.set(item_index, 1);
        }

        VectorWritable vw = new VectorWritable(userVector);
        vw.setWritesLaxPrecision(true);

        Text LuserId = new Text();
        LuserId.set(index.toString());

        context.write(LuserId, toObject(vw));
    }

}