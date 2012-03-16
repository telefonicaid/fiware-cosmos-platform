package es.tid.bdp.sftp.client;

import java.io.FileNotFoundException;
import java.io.IOException;

import com.jcraft.jsch.UserInfo;

import es.tid.bdp.sftp.PropertiesPlaceHolder;

public class PropUserInfo implements UserInfo {

    private static final String SFTP_PASSWORD = "sftp.password";
    private final String password;

    public PropUserInfo() throws FileNotFoundException, IOException {
        super();
        this.password = PropertiesPlaceHolder.getInstance().getProperty(
                SFTP_PASSWORD);
    }

    public String getPassphrase() {
        return this.password;
    }

    public String getPassword() {
        return this.password;
    }

    public boolean promptPassword(String message) {
        return true;
    }

    public boolean promptPassphrase(String message) {
        return false;
    }

    public boolean promptYesNo(String message) {
        return true;
    }

    public void showMessage(String message) {
    }
}
