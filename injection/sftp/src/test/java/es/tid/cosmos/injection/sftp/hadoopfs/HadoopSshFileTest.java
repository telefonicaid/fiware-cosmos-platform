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

package es.tid.cosmos.injection.sftp.hadoopfs;

import java.io.*;
import java.net.URI;
import java.util.List;

import com.google.common.io.Files;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.security.AccessControlException;
import org.apache.sshd.server.SshFile;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.mockito.Matchers;

import es.tid.cosmos.base.util.Logger;
import es.tid.cosmos.injection.sftp.BaseSftpTest;

import static org.junit.Assert.*;
import static org.mockito.Mockito.*;

/**
 * @author logc
 */
public class HadoopSshFileTest extends BaseSftpTest {

    private static final org.apache.log4j.Logger LOGGER =
            Logger.get(HadoopSshFile.class);
    private static final String USERNAME = System.getProperty("user.name");
    private static final String DEFAULT_FILESYSTEM_NAME = "fs.default.name";
    private static final String FILE_URI_PREAMBLE = "file:///";
    private static final String CYGWIN_FOLDER = "cygwin";

    private HadoopSshFile hadoopSshFile;
    private HadoopSshFile hadoopSshDir;
    private FileSystem hadoopFS;
    private FileSystem mockedFileSystem;
    private HadoopSshFile neverExists;
    private File tempDir;

    public HadoopSshFileTest() {
        super(LOGGER);
    }

    @Before
    public void setUp() throws IOException, InterruptedException{
        Configuration configuration = new Configuration();
        this.tempDir = Files.createTempDir();
        boolean success = this.tempDir.setWritable(true, false);

        /*
         * ignore error condition if running under Windows with cygwin
         * because apparently it is close to impossible to change folder
         * permissions under win7 and later versions
         */
        if (!success &&
                !this.tempDir.getAbsolutePath().contains(CYGWIN_FOLDER)) {
            throw new IllegalStateException("could not set to writable: " +
                    this.tempDir.toString());
        }

        configuration.set(
                DEFAULT_FILESYSTEM_NAME, FILE_URI_PREAMBLE
                + this.tempDir.toURI());
        String foodir = createUserTempURI("").toString();
        this.hadoopFS = FileSystem.get(configuration);
        this.hadoopSshDir = new HadoopSshFile(foodir, USERNAME, this.hadoopFS);
        this.hadoopSshFile = new HadoopSshFile(foodir + "/file01",
                USERNAME, this.hadoopFS);

        this.mockedFileSystem = mock(FileSystem.class);

        this.neverExists = new HadoopSshFile("/in/fantasy", "whatever_user",
                this.mockedFileSystem);
    }

    @After
    public void tearDown() throws IOException {
        if (this.hadoopSshFile.doesExist()) {
            this.hadoopSshFile.handleClose();
            assertTrue(this.hadoopSshFile.delete());
        }
        if (this.hadoopSshDir.doesExist()) {
            assertTrue(this.hadoopSshDir.delete());
        }
        boolean success = this.tempDir.delete();
        if (!success) {
            throw new IllegalStateException("could not delete: " +
                    this.tempDir.toString());
        }
        this.hadoopFS.close();
        this.mockedFileSystem.close();
    }

    @Test
    public void testGetAbsolutePath() throws Exception {
        assertEquals(this.tempDir.toURI().toString().concat(USERNAME)
                .concat("/file01"), this.hadoopSshFile.getAbsolutePath());
    }

    @Test
    public void testGetName() throws Exception {
        assertEquals("file01", this.hadoopSshFile.getName());
    }

    /**
     * Show that the owner is the logged in user as long as the file does not
     * exist. This complies with clients that request information about a path
     * before creating it.
     *
     * The owner is the username of the Java process after creation, since these
     * tests use the native filesystem underneath.
     *
     * @throws Exception
     */
    @Test
    public void testGetOwner() throws Exception {
        assertEquals(USERNAME, this.hadoopSshFile.getOwner());
        assertFalse(this.hadoopSshFile.doesExist());
        this.hadoopSshFile.create();
        assertNotSame(USERNAME, this.hadoopSshFile.getOwner());
    }

    @Test
    public void testIsDirectory() throws Exception {
        this.hadoopSshDir.mkdir();
        assertTrue(this.hadoopSshDir.isDirectory());
    }

    @Test
    public void testIsDirectoryDoesNotThrowException() throws Exception {
        assertFalse(this.hadoopSshDir.doesExist());
        assertFalse(this.hadoopSshDir.isDirectory());
    }

    @Test
    public void testDoesExist() throws Exception {
        assertFalse(this.hadoopSshFile.doesExist());
        this.hadoopSshFile.create();
        assertTrue(this.hadoopSshFile.doesExist());
    }

    /**
     * Show that method doesExist does not send an exception when it encounters
     * an Exception
     */
    @Test
    public void testExceptionDoesExist() throws Exception {
        when(this.mockedFileSystem.exists(Matchers.<Path>any()))
                .thenThrow(new IOException("you have been mocked"));
        assertFalse(this.neverExists.doesExist());
    }

    @Test
    public void testIsReadable() throws Exception {
        this.hadoopSshFile.create();
        assertTrue(this.hadoopSshFile.isReadable());
    }

    @Test
    public void testIsFile() throws Exception {
        this.hadoopSshFile.create();
        assertTrue(this.hadoopSshFile.isFile());
    }

    /**
     * Show that when isFile finds an IOException, e.g. the file is not found,
     * the function returns false instead of re-throwing the exception.
     *
     * @throws Exception
     */
    @Test
    public void testIsFileDoesNotThrowException() throws Exception {
        when(this.mockedFileSystem.isFile(Matchers.<Path>any()))
                .thenThrow(new IOException("you have been mocked"));
        assertFalse(this.neverExists.isFile());
    }

    @Test
    public void testIsWritable() throws Exception {
        this.hadoopSshFile.create();
        assertTrue(this.hadoopSshFile.isWritable());
    }

    /**
     * When a path does not yet exist, the write permission of its parent path
     * is returned.
     *
     * @throws Exception
     */
    @Test
    public void testIsWritableWhenNotYetExisting() throws Exception {
        this.hadoopSshDir.create();
        assertTrue(this.hadoopSshDir.isWritable());
        assertFalse(this.hadoopSshFile.doesExist());
        assertTrue(this.hadoopSshFile.isWritable());
    }

    /**
     * Show that there is no sense of an executable permission in HDFS. Anything
     * is not executable before creation, and executable afterwards,
     * even directories.
     *
     * @throws Exception
     */
    @Test
    public void testIsExecutable() throws Exception {
        assertFalse(this.hadoopSshFile.isExecutable());
        this.hadoopSshDir.mkdir();
        assertTrue(this.hadoopSshDir.isExecutable());
    }

    @Test
    public void testIsRemovable() throws Exception {
        this.hadoopSshFile.create();
        assertTrue(this.hadoopSshFile.isRemovable());
    }

    @Test
    public void testGetParentFile() throws Exception {
        this.hadoopSshFile.create();
        assertNotNull(this.hadoopSshFile.getParentFile());
    }

    @Test
    public void testGetLastModified() throws Exception {
        this.hadoopSshFile.create();
        assertNotSame(0, this.hadoopSshFile.getLastModified());
    }

    @Test
    public void testSetLastModified() throws Exception {
        this.hadoopSshFile.create();
        long fixedTime = System.currentTimeMillis();
        this.hadoopSshFile.setLastModified(fixedTime);
        // This assertion is not assertEquals because there is a precision
        // mismatch between HDFS and System.currentTimeMillis; we try to
        // write with more decimal places than can be read. What we can say is
        // that this difference has an upper limit.
        long retrievedTime = this.hadoopSshFile.getLastModified();
        assertTrue(String.format("sent: %s, got: %s, diff: %s", fixedTime,
                                 retrievedTime, fixedTime - retrievedTime),
                   Math.abs(fixedTime - retrievedTime) < 1000);
    }

    @Test
    public void testSetLastModifiedDoesNotThrowException() throws Exception {
        doThrow(new IOException("times could not be set"))
                .when(this.mockedFileSystem).setTimes(
                        Matchers.<Path>any(),
                        Matchers.anyLong(),
                        Matchers.anyLong());
        assertFalse(this.neverExists.setLastModified(123L));
    }

    @Test
    public void testGetSize() throws Exception {
        this.hadoopSshFile.create();
        OutputStream ostream = this.hadoopSshFile.createOutputStream(0);
        ostream.write("Hello world".getBytes());
        ostream.close();
        assertEquals(11, this.hadoopSshFile.getSize());
    }

    @Test
    public void testGetSizeDoesNotThrowException() throws Exception {
        when(this.mockedFileSystem.getFileStatus(Matchers.any(Path.class)))
                .thenThrow(new IOException("mocked"));
        this.neverExists.create();
        assertEquals(0L, this.neverExists.getSize());
    }

    @Test
    public void testMkdir() throws Exception {
        assertTrue(this.hadoopSshDir.mkdir());
    }

    @Test
    public void testMkdirDoesNotThrowException() throws Exception {
        when(this.mockedFileSystem.mkdirs(Matchers.<Path>any()))
                .thenThrow(new IOException("could not create dir"));
        this.neverExists.create();
        assertFalse(this.neverExists.mkdir());
    }

    @Test
    public void testDelete() throws Exception {
        this.hadoopSshFile.create();
        assertTrue(this.hadoopSshFile.doesExist());
        assertTrue(this.hadoopSshFile.delete());
        assertFalse(this.hadoopSshFile.doesExist());
    }

    @Test
    public void testDeleteDoesNotThrowException() throws Exception {
        HadoopSshFile mockFile = mock(HadoopSshFile.class);
        when(mockFile.isDirectory()).thenReturn(true);
        mockFile.create();
        when(this.mockedFileSystem.delete(Matchers.<Path>any(),
                                          Matchers.anyBoolean()))
                .thenThrow(new IOException("could not delete path"));
        assertFalse(mockFile.delete());
    }

    @Test
    public void testCreate() throws Exception {
        assertTrue(this.hadoopSshFile.create());
        assertTrue(this.hadoopSshFile.doesExist());
    }

    @Test
    public void testTruncate() throws Exception {
        this.hadoopSshFile.create();
        OutputStream ostream = this.hadoopSshFile.createOutputStream(0);
        ostream.write("Hello world".getBytes());
        ostream.close();
        assertEquals(11, this.hadoopSshFile.getSize());
        this.hadoopSshFile.truncate();
        assertEquals(0, this.hadoopSshFile.getSize());
    }

    @Test
    public void testMove() throws Exception {
        String newsubdir = createUserTempURI("/new/file01").toString();
        HadoopSshFile newfoo = new HadoopSshFile(
                newsubdir, USERNAME, this.hadoopFS);
        this.hadoopSshFile.create();
        assertFalse(newfoo.doesExist());
        this.hadoopSshFile.move(newfoo);
        assertTrue(newfoo.doesExist());
    }

    @Test
    public void testMoveDoesNotThrowException() throws Exception {
        when(this.mockedFileSystem.rename(Matchers.<Path>any(),
                Matchers.<Path>any())).thenThrow(new IOException("could not " +
                "rename this path"));
        assertFalse(this.neverExists.move(new HadoopSshFile("/wherever",
                    "some_user", this.mockedFileSystem)));
    }

    @Test
    public void testListSshFiles() throws Exception {
        this.hadoopSshFile.create();
        List<SshFile> fileList = this.hadoopSshDir.listSshFiles();
        assertEquals(1, fileList.size());
        SshFile found = fileList.get(0);
        assertEquals(this.hadoopSshFile.getAbsolutePath(), found.getAbsolutePath());
    }

    @Test
    public void testListFilesWhenDirectoryNotReadable() throws Exception {
        when(this.mockedFileSystem.listStatus(Matchers.<Path>any()))
            .thenThrow(new AccessControlException("not authorized"));
        HadoopSshFile mockedDir = spy(this.neverExists);
        doReturn(true).when(mockedDir).isDirectory();
        // There is only one object in the parent folder, namely this folder
        assertEquals(1, mockedDir.listSshFiles().size());
    }

    @Test
    public void testWriteToFile() throws Exception {
        this.hadoopSshFile.create();
        OutputStream ostream = this.hadoopSshFile.createOutputStream(0L);
        ostream.write("Hello world".getBytes());
        this.hadoopSshFile.handleClose();
        assertEquals(11, this.hadoopSshFile.getSize());
    }

    @Test
    public void testReadFromFile() throws Exception {
        this.hadoopSshFile.create();
        InputStream istream = this.hadoopSshFile.createInputStream(0L);
        int read = istream.read();
        istream.close();
        assertEquals(-1, read);

        String written = "Hello world";
        OutputStream outputStream = this.hadoopSshFile.createOutputStream(0L);
        outputStream.write(written.getBytes());
        this.hadoopSshFile.handleClose();
        InputStream inputStream = this.hadoopSshFile.createInputStream(0L);
        StringWriter writer = new StringWriter();
        int byteRead;
        while ((byteRead = inputStream.read()) != -1) {
            writer.write(byteRead);
        }
        assertEquals(written, writer.toString());
    }

    private URI createUserTempURI(String pathExtension) {
        return URI.create(
                this.tempDir.toURI().toString().concat(
                        USERNAME + pathExtension));
    }
}
