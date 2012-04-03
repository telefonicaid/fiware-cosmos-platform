package es.tid.bdp.e2etests;

import java.io.File;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import static org.testng.Assert.assertEquals;
import org.testng.annotations.*;

import es.tid.bdp.joblaunchers.Environment;
import es.tid.bdp.joblaunchers.FrontendLauncher;
import es.tid.bdp.joblaunchers.JobLauncher;
import es.tid.bdp.joblaunchers.TestException;

public class WordCountTest {
    public class TestImpl {
        private String text;
        private String inputFilePath;
        private final Map<String,Integer> expectedResult;
        private final JobLauncher jobLauncher;
        private final String jarPath;
        
        public TestImpl(JobLauncher launcher, String text, String wordcountJarPath) {
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
            this.jobLauncher = launcher;
            this.jarPath = wordcountJarPath;
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
        public void wordCountTest() throws IOException, TestException {
            String taskId = this.jobLauncher.createNewTask(this.inputFilePath,
                                                           this.jarPath);
            this.jobLauncher.waitForTaskCompletion(taskId);
            List<Map<String, String>> results = this.jobLauncher
                    .getResults(taskId);            
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
    
    private static final String WORDCOUNT_FILE = "wordcount-core-0.3.0.0-SNAPSHOT.jar";
    
    private Environment environment;
    private String wordcountPath;
    
    public Object[] testCreator(JobLauncher launcher) {
        final String word = "Word ";
        final int repetitions = 1000000;
        StringBuilder longStr = new StringBuilder(repetitions * word.length());
        for (int i = 0; i < repetitions; ++i) {
            longStr.append(word);
        }
        
        return new Object[] {
            new TestImpl(launcher, "One", this.wordcountPath),
            new TestImpl(launcher, "Two words", this.wordcountPath),
            new TestImpl(launcher, "Some text\n\t\nwith\tnon-space whitespace", this.wordcountPath),
            new TestImpl(launcher, longStr.toString(), this.wordcountPath)
        };
    }
    
    @Parameters({"environment", "jarPath"})
    @BeforeClass
    public void setup(String environment, String jarPath) {
        this.environment = Environment.valueOf(environment);
        this.wordcountPath = new File(jarPath, WORDCOUNT_FILE).getAbsolutePath();
    }
    
    @Factory
    public Object[] testsUI() {
        return testCreator(new FrontendLauncher(this.environment));
    }
}
