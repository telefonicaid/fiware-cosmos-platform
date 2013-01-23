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

package es.tid.cosmos.tests.e2e;

import java.io.File;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import static org.testng.Assert.assertEquals;
import org.testng.annotations.*;

import es.tid.cosmos.tests.environment.Environment;
import es.tid.cosmos.tests.hadoopjars.HadoopJars;
import es.tid.cosmos.tests.hadoopjars.JarNames;
import es.tid.cosmos.tests.tasks.FrontendTaskCreator;
import es.tid.cosmos.tests.tasks.Task;
import es.tid.cosmos.tests.tasks.TaskCreator;

public class WordCountIT {
    private static final String WORDCOUNT_PATH = HadoopJars.getPath(
            JarNames.Wordcount);

    public class TestImpl {
        private String text;
        private String inputFilePath;
        private final Map<String, Integer> expectedResult;
        private final TaskCreator taskCreator;

        public TestImpl(TaskCreator taskCreator, String text) {
            this.expectedResult = new HashMap<String, Integer>();
            this.text = text;
            String[] split = this.text.split("\\s+");
            for (String word : split) {
                int count = 0;
                if (this.expectedResult.containsKey(word)) {
                    count = this.expectedResult.get(word);
                }

                this.expectedResult.put(word, count + 1);
            }
            this.taskCreator = taskCreator;
        }

        @BeforeClass
        public void setup() throws IOException {
            File inputData = File.createTempFile("wordcount-", ".tmp");
            this.inputFilePath = inputData.getAbsolutePath();
            PrintWriter writer = new PrintWriter(inputData);
            try {
                writer.write(this.text);
            } finally {
                writer.close();
            }

            this.text = null;
        }

        @AfterClass
        public void cleanup() throws IOException {
            File inputData = new File(this.inputFilePath);
            inputData.delete();
            this.inputFilePath = null;
        }

        @Test
        public void wordCountTest() throws IOException {
            Task task = this.taskCreator.createTask(this.inputFilePath,
                                                    WORDCOUNT_PATH);
            task.run();
            task.waitForCompletion();
            List<Map<String, String>> results = task.getResults();
            for (Map<String, String> result : results) {
                assertEquals(result.size(),
                             2,
                             "Verifying each row has 2 elements");
                String word = result.get("_id");
                int count = Integer.parseInt(result.get("value"));
                assertEquals(count,
                             (int) this.expectedResult.get(word),
                             "Verifying count for word "
                        + word + "is the expected value");
            }
        }
    }

    public Object[] testCreator(TaskCreator taskCreator) {
        final String word = "Word ";
        final int repetitions = 1000000;
        StringBuilder longStr = new StringBuilder(repetitions * word.length());
        for (int i = 0; i < repetitions; ++i) {
            longStr.append(word);
        }

        return new Object[]{
                    new TestImpl(taskCreator, "One"),
                    new TestImpl(taskCreator, "Two words"),
                    new TestImpl(taskCreator, "Some text\n\t\nwith\tnon-space whitespace"),
                    new TestImpl(taskCreator, longStr.toString())
                };
    }

    @Parameters("environment")
    @Factory
    public Object[] testsUI(String environment) {
        return testCreator(new FrontendTaskCreator(Environment.valueOf(environment)));
    }
}
