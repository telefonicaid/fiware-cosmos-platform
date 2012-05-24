package es.tid.cosmos.platform.injection.server;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.net.URI;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.junit.Before;
import org.junit.Test;
import static junit.framework.Assert.assertEquals;
import static junit.framework.Assert.assertFalse;
import static org.mockito.Mockito.doThrow;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

/**
 * HadoopFileSystemViewTest
 *
 * @author logc
 * @since 15/05/12
 */
public class HadoopFileSystemViewTest {
    private String userName;
    private Configuration conf;
    private HadoopFileSystemView hadoopFileSystemView;

    @Before
    public void setUp() throws Exception {
        this.userName = "test";
        this.conf = new Configuration();
        this.hadoopFileSystemView = new HadoopFileSystemView(userName, conf);
    }

    @Test
    public void testGetFileWithEmptyFilename() throws Exception {
        HadoopSshFile file = this.hadoopFileSystemView.getFile("");
        FileSystem hadoopFS = FileSystem.get(
                URI.create(this.conf.get("fs.default.name")),
                           this.conf, this.userName);
        String homePath = hadoopFS.getHomeDirectory().toString()
                .replaceFirst(hadoopFS.getUri().toString(), "");
        assertEquals(homePath, file.getAbsolutePath());
        assertEquals(homePath.substring(homePath.lastIndexOf("/") + 1),
                     file.getName());
    }

    @Test
    public void testGetFile() throws Exception {
        HadoopSshFile bar = this.hadoopFileSystemView.getFile("/foo/bar");
        assertEquals("bar", bar.getName());
        assertEquals("/foo/bar", bar.getAbsolutePath());
        assertFalse(bar.doesExist());

        HadoopSshFile bq = this.hadoopFileSystemView.getFile(bar, "bq");
        assertEquals("bq", bq.getName());
        assertEquals("/foo/bar/bq", bq.getAbsolutePath());
        assertFalse(bq.doesExist());
    }

    @Test
    public void testRedirectionToHomePath() throws Exception {
        Configuration conf = new Configuration();
        FileSystem fs = FileSystem.get(URI.create(conf.get("fs.default.name")),
                conf, "test");
        String homePath = fs.getHomeDirectory().toString().replaceFirst(
                fs.getUri().toString(), "");
        HadoopSshFile init = this.hadoopFileSystemView.getFile(
                new HadoopSshFile(".", "test", fs), ".");
        assertEquals(homePath, init.getAbsolutePath());
    }
}