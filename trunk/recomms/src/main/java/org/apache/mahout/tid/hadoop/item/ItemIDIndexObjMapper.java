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

package org.apache.mahout.tid.hadoop.item;

import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.mahout.cf.taste.hadoop.TasteHadoopUtils;
import org.apache.mahout.cf.taste.hadoop.ToEntityPrefsMapper;
import org.apache.mahout.math.VarIntWritable;
import org.apache.mahout.math.VarLongWritable;
import org.bson.BSONObject;

public final class ItemIDIndexObjMapper extends
		Mapper<Object, BSONObject, VarIntWritable, VarLongWritable> {
	public boolean transpose;

	@Override
	protected void setup(Context context) {
		Configuration jobConf = context.getConfiguration();
		transpose = jobConf.getBoolean(ToEntityPrefsMapper.TRANSPOSE_USER_ITEM,
				false);
	}

	@Override
	protected void map(Object key, BSONObject value, Context context)
			throws IOException, InterruptedException {
		long itemID = Long.valueOf(value.get("item_id").toString());

		int index = TasteHadoopUtils.idToIndex(itemID);
		context.write(new VarIntWritable(index), new VarLongWritable(itemID));
	}
}
