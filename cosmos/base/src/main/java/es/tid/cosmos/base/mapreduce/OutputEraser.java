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

package es.tid.cosmos.base.mapreduce;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.OutputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;

/**
 *
 * @author ximo
 */
abstract class OutputEraser {
    private static final Map<Class<? extends OutputFormat>,
                             Class<? extends OutputEraser>> ERASERS =
        new HashMap() {{
            put(FileOutputFormat.class, FileDataEraser.class);
        }};

    public static OutputEraser getEraser(Class c) {
        if (!ERASERS.containsKey(c)) {
            Class superClass = c.getSuperclass();
            return superClass == null ? null : getEraser(superClass);
        }
        try {
            return ERASERS.get(c).newInstance();
        } catch (InstantiationException ex) {
            throw new IllegalArgumentException(
                    "Could not create instance of eraser class", ex);
        } catch (IllegalAccessException ex) {
            throw new IllegalArgumentException(
                    "Could not create instance of eraser class", ex);
        }
    }

    public abstract void deleteOutput(Job job) throws IOException;
}
