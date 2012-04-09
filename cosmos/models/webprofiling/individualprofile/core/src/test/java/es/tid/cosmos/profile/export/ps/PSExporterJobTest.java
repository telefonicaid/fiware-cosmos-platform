package es.tid.cosmos.profile.export.ps;

import java.io.File;
import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.TemporaryFolder;

/**
 *
 * @author dmicol, sortega
 */
public class PSExporterJobTest {
    private PSExporterJob instance;

    @Rule
    public TemporaryFolder outFolder = new TemporaryFolder();

    @Before
    public void setUp() throws IOException {
        this.instance = new PSExporterJob(new Configuration());
    }

    @Test
    public void testConfigure() throws IOException {
        Path path1 = new Path("file:/opt/test1");
        Path path2 = new Path("file:/opt/test2");
        this.instance.configure(path1, path2);
        Path[] inputPaths = FileInputFormat.getInputPaths(this.instance);
        assertEquals(1, inputPaths.length);
        assertEquals(path1, inputPaths[0]);
        assertEquals(path2, FileOutputFormat.getOutputPath(this.instance));
    }

    @Test
    public void shouldRenameOutputFile() throws Exception {
        this.instance.configure(new Path("file:/opt/input"),
                                new Path("file:" + outFolder.getRoot()));
        this.instance.getConfiguration().setLong("psexport.timestamp",
                                                 1331830825230L);
        File partFile = outFolder.newFile("part-r-0000.data");
        this.instance.renameOutput();
        assertFalse(partFile.exists());
        File expectedFile = new File(outFolder.getRoot(),
                                     "psprofile_BDP_20120315-180025.dat");
        assertTrue(expectedFile.exists());
    }
}
