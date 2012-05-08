package es.tid.cosmos.platform.injection.server;

import java.io.IOException;
import java.io.OutputStream;

import org.apache.hadoop.fs.Path;
import org.junit.After;
import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.ExpectedException;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * @author logc
 */
public class HadoopSshFileTest {
    private Logger LOG = LoggerFactory.getLogger(HadoopSshFileTest.class);
    private Path foo;
    private Path dir;
    private HadoopSshFile hfoo;
    private HadoopSshFile hdir;

    @Rule
    public ExpectedException exception = ExpectedException.none();

    @Before
    public void setUp() {
        this.dir = new Path("/tmp/user01");
        this.foo = new Path("/tmp/user01/file01");
        try {
            this.hfoo = new HadoopSshFile("/tmp/user01/file01",
                    "user01", 1);
            this.hdir = new HadoopSshFile("/tmp/user01/",
                    "user01", 1);
        } catch (IOException e) {
            LOG.error(e.getLocalizedMessage());
        }
    }

    @After
    public void tearDown() {
        this.hfoo.delete();
        this.hdir.delete();
    }

    @Test
    public void testGetAbsolutePath() throws Exception {
        assertEquals("/tmp/user01/file01", this.hfoo.getAbsolutePath());
    }

    @Test
    public void testGetName() throws Exception {
        assertEquals("file01", this.hfoo.getName());
    }

    @Test()
    public void testIsDirectory() throws Exception {
        //exception.expect(FileNotFoundException.class);
        // TODO: do not catch exception or check stack trace
        this.hdir.isDirectory();
    }

    @Test()
    public void testIsFile() throws Exception {
        //exception.expect(FileNotFoundException.class);
        // TODO: do not catch exception or check stack trace
        this.hfoo.isFile();
    }

    @Test
    public void testDoesExist() throws Exception {
        assertFalse(this.hfoo.doesExist());
        this.hfoo.create();
        assertTrue(this.hfoo.doesExist());
    }

    @Test
    public void testIsReadable() throws Exception {
        // TODO: implement testIsReadable

    }

    @Test
    public void testIsWritable() throws Exception {
        // TODO: testIsWritable
    }

    @Test
    public void testIsExecutable() throws Exception {
        assertFalse(this.hfoo.isExecutable());
        assertFalse(this.hdir.isExecutable());
    }

    @Test
    public void testIsRemovable() throws Exception {
        // TODO: testIsRemovable
    }

    @Test
    public void testGetParentFile() throws Exception {
        // TODO: testGetParentFile
    }

    @Test
    public void testGetLastModified() throws Exception {
        // TODO: testGetLastModified
    }

    @Test
    public void testSetLastModified() throws Exception {
        // TODO: testSetLastModified
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
        // TODO: do not catch exception or check stack trace
        this.hdir.mkdir();
    }

    @Test
    public void testDelete() throws Exception {
        this.hfoo.create();
        assertTrue(this.hfoo.doesExist());
        this.hfoo.delete();
        assertFalse(this.hfoo.doesExist());
    }

    @Test
    public void testCreate() throws Exception {
        this.hfoo.create();
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
        // TODO: testMove
    }

    @Test
    public void testListSshFiles() throws Exception {
        // TODO: testListSshFiles
    }

    @Test
    public void testCreateOutputStream() throws Exception {
        // TODO: testCreateOutputStream
    }

    @Test
    public void testCreateInputStream() throws Exception {
        // TODO: testCreateInputStream
    }

    @Test
    public void testHandleClose() throws Exception {
        // TODO: testHandleClose
    }
}
