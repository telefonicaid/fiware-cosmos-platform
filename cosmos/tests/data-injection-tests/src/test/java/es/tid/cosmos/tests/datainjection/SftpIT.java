package es.tid.cosmos.tests.datainjection;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.FutureTask;

import com.jcraft.jsch.*;
import org.apache.hadoop.fs.Path;
import static org.testng.Assert.assertEquals;
import org.testng.annotations.BeforeClass;
import org.testng.annotations.Parameters;
import org.testng.annotations.Test;

import es.tid.cosmos.tests.tasks.Environment;
import es.tid.cosmos.tests.tasks.EnvironmentSetting;

public class SftpIT {
    private Environment env;

    @Parameters("environment")
    @BeforeClass
    public void setup(String environment) throws IOException {
        this.env = Environment.valueOf(environment);
    }

    private Session connectToServer(String username, String password)
            throws JSchException {
        JSch jsch = new JSch();
        Session session = jsch.getSession(
                username,
                env.getProperty(EnvironmentSetting.SFTP_URL),
                22);

        session.setConfig("StrictHostKeyChecking", "no");
        session.setPassword(password);
        session.connect();
        return session;
    }

    private Session connectToServer()
            throws JSchException {
        return this.connectToServer(
                env.getProperty(EnvironmentSetting.DEFAULT_USER),
                env.getProperty(EnvironmentSetting.DEFAULT_PASSWORD));
    }

    private ChannelSftp connectToSftp(Session session) throws JSchException {
        Channel channel = session.openChannel("sftp");
        channel.connect();
        return (ChannelSftp)channel;
    }

    @Test
    public void testFileUpload(String environment)
            throws SftpException, JSchException, IOException {
        final Session session = this.connectToServer();
        try {
            this.putAndVerifyFile(session, "testFileUpload", new Data(1));
        } finally {
            session.disconnect();
        }
    }

    @Test(expectedExceptions = SftpException.class)
    public void testOutOfDirectoryUpload1(String environment)
            throws SftpException, JSchException, IOException {
        final Session session = this.connectToServer();
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
    public void testOutOfDirectoryUpload2(String environment)
            throws SftpException, JSchException, IOException {
        final Session session = this.connectToServer();
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
    public void testOutOfDirectoryUpload3(String environment)
            throws SftpException, JSchException, IOException {
        final Session session = this.connectToServer();
        try {
            this.putAndVerifyFile(
                    session,
                    "/testOutOfDirectoryUpload",
                    new Data(1));
        } finally {
            session.disconnect();
        }
    }

    @Test
    public void testFileUploadWithEscaping1(String environment)
            throws SftpException, JSchException, IOException {
        final Session session = this.connectToServer();
        try {
            this.putAndVerifyFile(
                    session,
                    "./testEscapedUpload",
                    new Data(Byte.MAX_VALUE / 4));
        } finally {
            session.disconnect();
        }
    }

    @Test
    public void testFileUploadWithEscaping2(String environment)
            throws SftpException, JSchException, IOException {
        final Session session = this.connectToServer();
        final ChannelSftp sftpChannel = this.connectToSftp(session);
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
    public void testMoveToRoot(String environment)
            throws SftpException, JSchException, IOException {
        final Session session = this.connectToServer();
        final ChannelSftp sftpChannel = this.connectToSftp(session);
        try {
            sftpChannel.cd("/");
        } finally {
            sftpChannel.exit();
            session.disconnect();
        }
    }

    @Test
    public void testDefaultPwd(String environment)
            throws SftpException, JSchException, IOException {
        final Session session = this.connectToServer();
        final ChannelSftp sftpChannel = this.connectToSftp(session);
        try {
            assertEquals(sftpChannel.pwd(), this.getDefaultDir());
        } finally {
            sftpChannel.exit();
            session.disconnect();
        }
    }

    @Test
    public void testDirCommands(String environment)
            throws SftpException, JSchException, IOException {
        final Session session = this.connectToServer();
        final ChannelSftp sftpChannel = this.connectToSftp(session);
        final String dirName = "myTempDir";
        try {
            sftpChannel.mkdir(dirName);
            sftpChannel.cd(dirName);
            assertEquals(sftpChannel.ls(".").size(), 2); // "." and ".."
            sftpChannel.cd("..");
            sftpChannel.rmdir(dirName);
        } finally {
            sftpChannel.exit();
            session.disconnect();
        }
    }

    private void putFile(ChannelSftp sftpChannel, String filePath,
                         Iterable<Integer> data)
            throws SftpException, IOException {
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
            throws SftpException, IOException {
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
            throws SftpException, IOException, JSchException {
        final ChannelSftp sftpChannel = this.connectToSftp(session);

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

    @Test
    public void testParallelFileUpload(String environment)
            throws SftpException, JSchException, IOException,
                   ExecutionException, InterruptedException {
        final Session session = this.connectToServer();
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
