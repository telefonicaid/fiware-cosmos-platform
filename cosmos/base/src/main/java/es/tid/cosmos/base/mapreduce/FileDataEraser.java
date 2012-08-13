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

import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;

/**
 *
 * @author ximo
 */
class FileDataEraser extends OutputEraser {
    @Override
    public void deleteOutput(Job job) throws IOException {
        FileSystem fs = FileSystem.get(job.getConfiguration());
        Path outputPath = FileOutputFormat.getOutputPath(job);
        if (!fs.deleteOnExit(outputPath)) {
            throw new IOException("Failed to delete output path "
                + outputPath.toString());
        }
    }
}
