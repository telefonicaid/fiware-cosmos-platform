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
            putAndVerifyFile(session, "testFileUpload", new Data(1));
        } finally {
            session.disconnect();
        }
    }

    // TODO: Change the expected exception to something more specific
    @Test(expectedExceptions=Exception.class)
    public void testOutOfDirectoryUpload(String environment)
            throws SftpException, JSchException, IOException {
        final Session session = this.connectToServer();
        try {
            putAndVerifyFile(session, "../testOutOfDirectoryUpload",
                             new Data(1));
        } finally {
            session.disconnect();
        }
    }

    private void putAndVerifyFile(Session session, String fileName,
                                  Iterable<Integer> data)
            throws SftpException, IOException, JSchException {
        final ChannelSftp sftpChannel = this.connectToSftp(session);

        try {
            final String currentDir = sftpChannel.pwd();
            final String filePath = currentDir + Path.SEPARATOR + fileName;

            // Put file in server
            OutputStream output = sftpChannel.put(filePath);
            try {
                for (int b : data) {
                    output.write(b);
                }
            } finally {
                output.close();
            }

            // Read file back from server
            InputStream input = sftpChannel.get(filePath);
            try {
                for (int b : data) {
                    assertEquals(input.read(), b);
                }
            } finally {
                input.close();
            }

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
                putAndVerifyFile(session, "testParallelFileUpload1",
                                 new Data(1));
                return null;
            }
        });
        FutureTask task2 = new FutureTask(new Callable() {
            @Override
            public Object call() throws Exception {
                putAndVerifyFile(session, "testParallelFileUpload2",
                                 new Data(5));
                return null;
            }
        });
        FutureTask task3 = new FutureTask(new Callable() {
            @Override
            public Object call() throws Exception {
                putAndVerifyFile(session, "testParallelFileUpload3",
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
