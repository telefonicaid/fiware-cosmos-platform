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

import java.io.File;
import java.io.FileFilter;
import java.io.IOException;
import java.io.PrintWriter;
import java.net.URL;
import java.nio.charset.Charset;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;
import java.util.TreeSet;

import org.apache.hadoop.thirdparty.guava.common.io.Files;
import org.apache.pig.ExecType;
import org.apache.pig.PigServer;
import org.junit.Before;
import org.junit.Test;

import static java.util.Arrays.asList;
import static org.junit.Assert.assertEquals;

public class PigScriptJobIntegrationTest {

    private PigScriptJob instance;
    private String outputDir;

    @Before
    public void setUp() throws Exception {
        PigServer srv = new PigServer(ExecType.LOCAL);

        File workersFile = createWorkersTempFile();
        File departmentsFile = createDepartmentsTempFile();
        this.outputDir = getOutputTempDir();

        Map<String, String> params = new HashMap<String, String>();
        params.put("workers_file", workersFile.getAbsolutePath());
        params.put("departments_file", departmentsFile.getAbsolutePath());
        params.put("output_dir", this.outputDir);

        URL scriptLocation = this.getClass().getResource("/it.pig");
        if (scriptLocation == null) {
            throw new IllegalStateException(
                    "cannot find integration test script");
        }
        this.instance = new PigScriptJob(srv, scriptLocation, params);
    }

    @Test
    public void shouldExecuteScript() throws Exception {
        this.instance.submit();
        this.instance.waitForCompletion(true);

        File output = new File(this.outputDir);
        File[] outputFiles = output.listFiles(new FileFilter() {
            @Override
            public boolean accept(File pathname) {
                return pathname.getName().startsWith("part");
            }
        });
        assertEquals(1, outputFiles.length);

        Set<String> obtained = new TreeSet<String>(Files.readLines
                (outputFiles[0],
                Charset.forName("UTF-8")));
        Set<String> expected = new TreeSet<String>(asList(
                "1,Mike,1,1,Development",
                "2,Rachel,2,2,UX",
                "3,Leonard,2,2,UX",
                "4,Lisa,3,3,Support"));
        assertEquals(expected, obtained);
    }

    private static File createWorkersTempFile() throws IOException {
        File file = File.createTempFile("workers-", ".csv");
        PrintWriter writer = new PrintWriter(file);
        writer.println("1,Mike, 1");
        writer.println("2,Rachel, 2");
        writer.println("3,Leonard, 2");
        writer.println("4,Lisa, 3");
        writer.close();
        return file;
    }
    private static File createDepartmentsTempFile() throws IOException {
        File file = File.createTempFile("departments-", ".csv");
        PrintWriter writer = new PrintWriter(file);
        writer.println("1,Development");
        writer.println("2,UX");
        writer.println("3,Support");
        writer.close();
        return file;
    }

    private static String getOutputTempDir() throws IOException {
        File dir = File.createTempFile("output-", "");
        dir.delete();
        return dir.getAbsolutePath();
    }
}
