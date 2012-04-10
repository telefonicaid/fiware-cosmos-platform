package es.tid.cosmos.platform.cluster.server;

import java.io.PrintWriter;
import java.io.StringWriter;
import java.util.Properties;

import javax.mail.Message;
import javax.mail.Session;
import javax.mail.Transport;
import javax.mail.internet.InternetAddress;
import javax.mail.internet.MimeMessage;

import org.apache.log4j.Logger;

/**
 *
 * @author dmicol
 */
final class ClusterServerUtil {
    private static final Logger LOG = Logger.getLogger(ClusterServer.class);
    
    static class ExitWithSuccessCodeException extends SecurityException { }
    static class ExitWithFailureCodeException extends SecurityException { }

    private ClusterServerUtil() {
    }
    
    static void allowExitCalls() {
        System.setSecurityManager(null);
    }
    
    static void disallowExitCalls() {
        final SecurityManager securityManager = new SecurityManager() {
            @Override
            public void checkPermission(java.security.Permission permission) {
                if (permission.getName().contains("exitVM.0")) {
                    throw new ExitWithSuccessCodeException();
                } else if (permission.getName().contains("exitVM")) {
                    throw new ExitWithFailureCodeException();
                }
            }
        };
        System.setSecurityManager(securityManager);
    }
    
    static String getFullExceptionInformation(Throwable exception) {
        return (exception.toString() + "\n" + getStackTrace(exception));
    }
    
    static String getStackTrace(Throwable exception) {
        StringWriter writer = new StringWriter();
        exception.printStackTrace(new PrintWriter(writer));
        return writer.toString();
    }

    static void logFatalError(String emailAddress, Exception exception) {
        logFatalException(exception);
        sendNotificationEmail(emailAddress, exception);
    }
    
    private static void logFatalException(Exception exception) {
        LOG.fatal(getFullExceptionInformation(exception));
    }
    
    private static void sendNotificationEmail(String emailAddress,
                                              Exception exception) {
        String text = "Cosmos failed in production :(\n\n"
                + "The error message was: " + exception.toString() + "\n"
                + "and the call stack:" + getStackTrace(exception) + "\n\n"
                + "Please fix me!\n";
        
        Properties props = new Properties();
        props.put("mail.smtp.starttls.enable", "false");
        props.put("mail.smtp.host", "mailhost.hi.inet");
        props.put("mail.smtp.port", "25");
        Session session = Session.getInstance(props, null);
        try {
            Message msg = new MimeMessage(session);
            msg.setFrom(new InternetAddress(emailAddress));
            msg.addRecipient(Message.RecipientType.TO,
                             new InternetAddress(emailAddress));
            msg.setSubject("Cosmos Failure");
            msg.setText(text);
            Transport.send(msg);
        } catch (Exception ex) {
            logFatalException(ex);
        }
    }
}
