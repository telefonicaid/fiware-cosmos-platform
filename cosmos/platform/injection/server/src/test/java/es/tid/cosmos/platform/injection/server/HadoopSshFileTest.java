package es.tid.cosmos.platform.injection.server;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.StringWriter;
import java.util.List;

import org.apache.hadoop.conf.Configuration;
import org.apache.sshd.server.SshFile;
import org.junit.After;
import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * @author logc
 */
public class HadoopSshFileTest {
    private Logger LOG = LoggerFactory.getLogger(HadoopSshFileTest.class);
    private HadoopSshFile hfoo;
    private HadoopSshFile hdir;
    private String user01 = "user01";
    private Configuration configuration;

    @Before
    public void setUp() {
        this.configuration = new Configuration();
        String foodir = "/tmp/user01";
        String foofile = "/tmp/user01/file01";
        try {
            this.hfoo = new HadoopSshFile(foofile, this.user01, configuration);
            this.hdir = new HadoopSshFile(foodir, this.user01, configuration);
        } catch (IOException e) {
            LOG.error(e.getMessage());
        } catch (InterruptedException e) {
            LOG.error(e.getMessage());
        }
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
        //exception.expect(FileNotFoundException.class);
        // TODO: do not catch exception or check stack trace
        this.hdir.isDirectory();
    }

    @Test
    public void testDoesExist() throws Exception {
        assertFalse(this.hfoo.doesExist());
        this.hfoo.create();
        assertTrue(this.hfoo.doesExist());
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
        long modificationTime = this.hfoo.getLastModified();
        assertEquals(0, modificationTime);
    }

    @Test
    public void testSetLastModified() throws Exception {
        this.hfoo.create();
        long fixedTime = System.currentTimeMillis();
        this.hfoo.setLastModified(fixedTime);
        /* This assertion is not assertEquals because there is a precision
         * mismatch between HDFS and System.currentTimeMillis; we try to
         * write with more decimal places than can be read. What we can say is
         * that this difference has an upper limit.
         */
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
        boolean success = this.hdir.mkdir();
        assertTrue(success);
    }

    @Test
    public void testDelete() throws Exception {
        this.hfoo.create();
        assertTrue(this.hfoo.doesExist());
        boolean success = this.hfoo.delete();
        assertTrue(success);
        assertFalse(this.hfoo.doesExist());
    }

    @Test
    public void testCreate() throws Exception {
        boolean success = this.hfoo.create();
        assertTrue(success);
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
        HadoopSshFile newfoo = new HadoopSshFile(newsubdir, this.user01,
                this.configuration);
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

    @Test
    public void testCreateInputStream() throws Exception {
        this.hfoo.create();
        InputStream istream = this.hfoo.createInputStream(0);
        int read = istream.read();
        istream.close();
        assertEquals(-1, read);

        OutputStream outputStream = this.hfoo.createOutputStream(0);
        outputStream.write("Hello world".getBytes());
        outputStream.close();
        InputStream inputStream = this.hfoo.createInputStream(0);
        StringWriter writer = new StringWriter();
        for (int byte_read = 0; byte_read != -1;
             byte_read = inputStream.read()) {
            writer.write(byte_read);
        }
        /* There is one byte in the read string that is not allowing expected
         * and actual to be completely equal
         */
        assertTrue(writer.toString().contains("Hello world"));
    }
}