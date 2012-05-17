package es.tid.cosmos.platform.injection.server;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.StringWriter;
import java.net.URI;
import java.util.List;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.sshd.server.SshFile;
import org.junit.After;
import static org.junit.Assert.*;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;
import org.junit.Before;
import org.junit.Ignore;
import org.junit.Test;
import org.mockito.Matchers;

import es.tid.cosmos.base.util.Logger;

/**
 * @author logc
 */
public class HadoopSshFileTest {
    private final org.apache.log4j.Logger LOG = Logger.get(HadoopSshFileTest.class);
    private HadoopSshFile hfoo;
    private HadoopSshFile hdir;
    private FileSystem hadoopFS;

    @Before
    public void setUp() throws IOException, InterruptedException{
        Configuration configuration = new Configuration();
        String foodir = "/tmp/user01";
        String foofile = "/tmp/user01/file01";
        this.hadoopFS = FileSystem.get(
                URI.create(configuration.get("fs.default.name")),
                configuration, "user01");
        this.hfoo = new HadoopSshFile(foofile, "user01", this.hadoopFS);
        this.hdir = new HadoopSshFile(foodir, "user01", this.hadoopFS);
    }

    @After
    public void tearDown() {
        if (this.hfoo.doesExist()) {
            this.hfoo.delete();
        }
        if (this.hdir.doesExist()) {
            this.hdir.delete();
        }
    }

    @Test
    public void testGetAbsolutePath() throws Exception {
        assertEquals("/tmp/user01/file01", this.hfoo.getAbsolutePath());
    }

    @Test
    public void testGetName() throws Exception {
        assertEquals("file01", this.hfoo.getName());
    }

    @Test
    public void testIsDirectory() throws Exception {
        this.hdir.mkdir();
        assertTrue(this.hdir.isDirectory());
    }

    @Test
    public void testDoesExist() throws Exception {
        assertFalse(this.hfoo.doesExist());
        this.hfoo.create();
        assertTrue(this.hfoo.doesExist());
    }

    @Test
    public void testExceptionDoesExist() throws Exception {
        FileSystem mocked = mock(FileSystem.class);
        when(mocked.exists(Matchers.<Path>any()))
                .thenThrow(new IOException("you have been mocked"));
        HadoopSshFile neverExists = new HadoopSshFile("/in/fantasyland",
                "whatever_user", mocked);
        assertFalse(neverExists.doesExist());
    }

    @Test
    public void testIsReadable() throws Exception {
        this.hfoo.create();
        assertTrue(this.hfoo.isReadable());
    }

    @Test
    public void testIsFile() throws Exception {
        this.hfoo.create();
        assertTrue(this.hfoo.isFile());
    }

    @Test
    public void testIsWritable() throws Exception {
        this.hfoo.create();
        assertTrue(this.hfoo.isWritable());
    }

    @Test
    public void testIsExecutable() throws Exception {
        assertFalse(this.hfoo.isExecutable());
        assertFalse(this.hdir.isExecutable());
    }

    @Test
    public void testIsRemovable() throws Exception {
        this.hfoo.create();
        assertTrue(this.hfoo.isRemovable());
    }

    @Test
    public void testGetParentFile() throws Exception {
        this.hfoo.create();
        assertTrue(this.hfoo.getParentFile().toString()
                .startsWith("es.tid.cosmos.platform.injection" +
                        ".server.HadoopSshFile@"));
    }

    @Test
    public void testGetLastModified() throws Exception {
        assertEquals(0, this.hfoo.getLastModified());
    }

    @Test
    public void testSetLastModified() throws Exception {
        this.hfoo.create();
        long fixedTime = System.currentTimeMillis();
        this.hfoo.setLastModified(fixedTime);
        // This assertion is not assertEquals because there is a precision
        // mismatch between HDFS and System.currentTimeMillis; we try to
        // write with more decimal places than can be read. What we can say is
        // that this difference has an upper limit.
        //
        assertTrue(fixedTime - this.hfoo.getLastModified() < 1000);
    }

    @Test
    public void testGetSize() throws Exception {
        this.hfoo.create();
        OutputStream ostream = this.hfoo.createOutputStream(0);
        ostream.write("Hello world".getBytes());
        ostream.close();
        assertEquals(11, this.hfoo.getSize());
    }

    @Test
    public void testMkdir() throws Exception {
        assertTrue(this.hdir.mkdir());
    }

    @Test
    public void testDelete() throws Exception {
        this.hfoo.create();
        assertTrue(this.hfoo.doesExist());
        assertTrue(this.hfoo.delete());
        assertFalse(this.hfoo.doesExist());
    }

    @Test
    public void testCreate() throws Exception {
        assertTrue(this.hfoo.create());
        assertTrue(this.hfoo.doesExist());
    }

    @Test
    public void testTruncate() throws Exception {
        this.hfoo.create();
        OutputStream ostream = this.hfoo.createOutputStream(0);
        ostream.write("Hello world".getBytes());
        ostream.close();
        assertEquals(11, this.hfoo.getSize());
        this.hfoo.truncate();
        assertEquals(0, this.hfoo.getSize());
    }

    @Test
    public void testMove() throws Exception {
        String newsubdir = "/tmp/user01/new/file01";
        HadoopSshFile newfoo = new HadoopSshFile(newsubdir, "user01",
                this.hadoopFS);
        this.hfoo.create();
        assertFalse(newfoo.doesExist());
        this.hfoo.move(newfoo);
        assertTrue(newfoo.doesExist());
    }

    @Test
    public void testListSshFiles() throws Exception {
        this.hfoo.create();
        List<SshFile> fileList = this.hdir.listSshFiles();
        assertEquals(1, fileList.size());
        SshFile found = fileList.get(0);
        assertEquals(this.hfoo.getAbsolutePath(), found.getAbsolutePath());
    }

    @Test
    public void testCreateOutputStream() throws Exception {
        this.hfoo.create();
        OutputStream ostream = this.hfoo.createOutputStream(0);
        ostream.write("Hello world".getBytes());
        ostream.close();
        assertEquals(11, this.hfoo.getSize());
    }

    @Ignore
    @Test
    public void testCreateInputStream() throws Exception {
        this.hfoo.create();
        InputStream istream = this.hfoo.createInputStream(0);
        int read = istream.read();
        istream.close();
        assertEquals(-1, read);

        String written = "Hello world";
        OutputStream outputStream = this.hfoo.createOutputStream(0);
        outputStream.write(written.getBytes());
        outputStream.close();
        InputStream inputStream = this.hfoo.createInputStream(0);
        StringWriter writer = new StringWriter();
        int byteRead = 0;
        while ((byteRead = inputStream.read()) != -1) {
            writer.write(byteRead);
        }
        assertEquals(written, writer.toString());
    }
}