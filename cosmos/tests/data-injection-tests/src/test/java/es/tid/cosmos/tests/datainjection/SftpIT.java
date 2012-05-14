package es.tid.cosmos.tests.datainjection;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.List;
import java.util.concurrent.Callable;
import java.util.concurrent.FutureTask;

import com.jcraft.jsch.ChannelSftp;
import com.jcraft.jsch.ChannelSftp.LsEntry;
import com.jcraft.jsch.JSchException;
import com.jcraft.jsch.Session;
import com.jcraft.jsch.SftpException;
import org.apache.hadoop.fs.Path;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertTrue;
import org.testng.annotations.BeforeClass;
import org.testng.annotations.Parameters;
import org.testng.annotations.Test;

import es.tid.cosmos.tests.sftp.om.CosmosSftp;
import es.tid.cosmos.tests.tasks.Environment;
import es.tid.cosmos.tests.tasks.EnvironmentSetting;

@Test
public class SftpIT {
    private Environment env;

    @Parameters("environment")
    @BeforeClass
    public void setup(String environment) throws IOException {
        this.env = Environment.valueOf(environment);
    }

    public void testFileUpload()
            throws Exception {
        final Session session = CosmosSftp.createSession(this.env);
        try {
            this.putAndVerifyFile(session, "testFileUpload", new Data(1));
        } finally {
            session.disconnect();
        }
    }

    public void testFileUploadOverwrite()
            throws Exception {
        final Session session = CosmosSftp.createSession(this.env);
        try {
            this.putAndVerifyFile(session, "testFileUpload", new Data(5));
            this.putAndVerifyFile(session, "testFileUpload", new Data(8));
        } finally {
            session.disconnect();
        }
    }

    @Test(expectedExceptions = SftpException.class)
    public void testOutOfDirectoryUpload1()
            throws Exception {
        final Session session = CosmosSftp.createSession(this.env);
        try {
            this.putAndVerifyFile(session, "../testOutOfDirectoryUpload",
                                  new Data(1));
        } finally {
            session.disconnect();
        }
    }

    private String getDefaultDir() {
        return "/user/" + this.env.getProperty(EnvironmentSetting.DEFAULT_USER);
    }

    @Test(expectedExceptions = SftpException.class)
    public void testOutOfDirectoryUpload2()
            throws Exception {
        final Session session = CosmosSftp.createSession(this.env);
        try {
            this.putAndVerifyFile(
                    session,
                    this.getDefaultDir() + "/../testOutOfDirectoryUpload",
                    new Data(1));
        } finally {
            session.disconnect();
        }
    }

    @Test(expectedExceptions = SftpException.class)
    public void testOutOfDirectoryUpload3()
            throws Exception {
        final Session session = CosmosSftp.createSession(this.env);
        try {
            this.putAndVerifyFile(
                    session,
                    "/testOutOfDirectoryUpload",
                    new Data(1));
        } finally {
            session.disconnect();
        }
    }

    public void testFileUploadWithEscaping1()
            throws Exception {
        final Session session = CosmosSftp.createSession(this.env);
        try {
            this.putAndVerifyFile(
                    session,
                    "./testEscapedUpload",
                    new Data(Byte.MAX_VALUE / 4));
        } finally {
            session.disconnect();
        }
    }

    public void testFileUploadWithEscaping2()
            throws Exception {
        final Session session = CosmosSftp.createSession(this.env);
        final ChannelSftp sftpChannel = CosmosSftp.connectToSftp(session);
        final String dirName = "myDumyDir";
        try {
            Data data = new Data(Byte.MAX_VALUE / 4);
            sftpChannel.mkdir(dirName);
            this.putFile(sftpChannel, dirName + "../testEscapedUpload", data);
            this.verifyFile(sftpChannel, dirName + "../testEscapedUpload", data);
            this.verifyFile(sftpChannel, "testEscapedUpload", data);
            sftpChannel.rmdir(dirName);
        } finally {
            sftpChannel.exit();
            session.disconnect();
        }
    }

    @Test(expectedExceptions = SftpException.class)
    public void testListRoot()
            throws Exception {
        final Session session = CosmosSftp.createSession(this.env);
        final ChannelSftp sftpChannel = CosmosSftp.connectToSftp(session);
        try {
            sftpChannel.ls("/");
        } finally {
            sftpChannel.exit();
            session.disconnect();
        }
    }

    public void testDefaultPwd()
            throws Exception {
        final Session session = CosmosSftp.createSession(this.env);
        final ChannelSftp sftpChannel = CosmosSftp.connectToSftp(session);
        try {
            assertEquals(sftpChannel.pwd(), this.getDefaultDir());
        } finally {
            sftpChannel.exit();
            session.disconnect();
        }
    }

    @Test(expectedExceptions = JSchException.class)
    public void testUserAuth1() throws Exception {
        CosmosSftp.createSession(
                this.env,
                this.env.getProperty(EnvironmentSetting.DEFAULT_USER),
                "BadPassword");
    }

    @Test(expectedExceptions = JSchException.class)
    public void testUserAuth2() throws Exception {
        CosmosSftp.createSession(this.env, "BadUser", "BadPassword");
    }

    @Test(expectedExceptions = JSchException.class)
    public void testUserAuth3() throws Exception {
        CosmosSftp.createSession(this.env, "root", "root");
    }

    @Test(expectedExceptions = JSchException.class)
    public void testUserAuth4() throws Exception {
        CosmosSftp.createSession(this.env, "root", "1234");
    }

    public void testDirCommands()
            throws Exception {
        final Session session = CosmosSftp.createSession(this.env);
        final ChannelSftp sftpChannel = CosmosSftp.connectToSftp(session);
        final String dirName = "myTempDir";
        try {
            sftpChannel.mkdir(dirName);
            sftpChannel.cd(dirName);
            List<LsEntry> ls = sftpChannel.ls(".");
            assertEquals(ls.size(), 2); // "." and ".."
            for (LsEntry entry : ls) {
                assertTrue(entry.getLongname().contains("rw"),
                           "Verifying ls returns permissions data");
            }
            sftpChannel.cd("..");
            sftpChannel.rmdir(dirName);
        } finally {
            sftpChannel.exit();
            session.disconnect();
        }
    }

    private void putFile(ChannelSftp sftpChannel, String filePath,
                         Iterable<Integer> data)
            throws Exception {
        OutputStream output = sftpChannel.put(filePath);
        try {
            for (int b : data) {
                output.write(b);
            }
        } finally {
            output.close();
        }
    }

    private void verifyFile(ChannelSftp sftpChannel, String filePath,
                            Iterable<Integer> data)
            throws Exception {
        InputStream input = sftpChannel.get(filePath);
        try {
            for (int b : data) {
                assertEquals(input.read(), b);
            }
        } finally {
            input.close();
        }
    }

    private void putAndVerifyFile(Session session, String fileName,
                                  Iterable<Integer> data)
            throws Exception {
        final ChannelSftp sftpChannel = CosmosSftp.connectToSftp(session);

        try {
            final String currentDir = sftpChannel.pwd();
            final String filePath = currentDir + Path.SEPARATOR + fileName;

            this.putFile(sftpChannel, filePath, data);
            this.verifyFile(sftpChannel, filePath, data);

            // Remove file
            sftpChannel.rm(filePath);
        } finally {
            sftpChannel.exit();
        }
    }

    public void testParallelFileUpload()
            throws Exception {
        final Session session = CosmosSftp.createSession(this.env);
        FutureTask task1 = new FutureTask(new Callable() {
            @Override
            public Object call() throws Exception {
                SftpIT.this.putAndVerifyFile(session, "testParallelFileUpload1",
                                             new Data(1));
                return null;
            }
        });
        FutureTask task2 = new FutureTask(new Callable() {
            @Override
            public Object call() throws Exception {
                SftpIT.this.putAndVerifyFile(session, "testParallelFileUpload2",
                                             new Data(5));
                return null;
            }
        });
        FutureTask task3 = new FutureTask(new Callable() {
            @Override
            public Object call() throws Exception {
                SftpIT.this.putAndVerifyFile(session, "testParallelFileUpload3",
                                             new Data(10));
                return null;
            }
        });

        try {
            task1.run();
            task2.run();
            task3.run();
            task1.get();
            task2.get();
            task3.get();
        } finally {
            session.disconnect();
        }
    }
}
