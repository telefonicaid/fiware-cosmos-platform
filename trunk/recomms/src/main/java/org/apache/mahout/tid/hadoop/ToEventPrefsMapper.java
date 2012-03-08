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
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.mahout.tid.hadoop.EventsWritable;
import org.apache.mahout.tid.impl.recommender.RecommenderTIDJob;
import org.apache.mahout.math.VarLongWritable;
import org.bson.BSONObject;
import java.io.IOException;

public class ToEventPrefsMapper extends
		Mapper<Object, Object, VarLongWritable, VarLongWritable> {
	private boolean booleanData;

	@Override
	protected void setup(Context context) {
		Configuration jobConf = context.getConfiguration();
		booleanData = jobConf.getBoolean(RecommenderTIDJob.BOOLEAN_DATA, false);
	}

	public enum event_types {
		purchase, rate, recodisplay, click, view
	};

	public ToEventPrefsMapper() {
	}

	@Override
	public void map(Object key, Object value, Context context)
			throws IOException, InterruptedException {

		long userID;
		long itemID;
		float eventValue;
		long eventType;
		String type = new String();
		if (value instanceof BSONObject) {
			BSONObject BSONvalue = (BSONObject) value;
			userID = Long.valueOf(BSONvalue.get("user_id").toString());
			itemID = Long.valueOf(BSONvalue.get("item_id").toString());
			eventValue = Float.parseFloat(BSONvalue.get("eventValue")
					.toString());
			type = BSONvalue.get("event_type").toString();
		} else {
			Text valueText = (Text) value;
			String[] st = valueText.toString().split(",");
			userID = Long.valueOf(st[0]);
			itemID = Long.valueOf(st[3]);
			eventValue = Float.parseFloat(st[4]);
			type = st[2];
		}

		event_types etype = Enum.valueOf(ToEventPrefsMapper.event_types.class,
				type.toString());

		switch (etype) {
		case purchase:
			eventType = 0;
			break;
		case rate:
			eventType = 1;
			break;
		case recodisplay:
			eventType = 2;
			break;
		case click:
			eventType = 3;
			break;
		case view:
			eventType = 4;
			break;
		default:
			eventType = 1;
			break;
		}

		if (booleanData) {
			context.write(new VarLongWritable(userID), new VarLongWritable(
					itemID));
		} else {
			try {
				context.write(new VarLongWritable(userID), new EventsWritable(
						itemID, eventType, eventValue));
			} catch (Exception e) {
				eventValue = 1.0f;
				context.write(new VarLongWritable(userID), new EventsWritable(
						itemID, eventType, eventValue));
			}

		}
	}

}
