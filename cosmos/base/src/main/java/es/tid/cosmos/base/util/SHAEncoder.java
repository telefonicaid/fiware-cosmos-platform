package es.tid.cosmos.base.util;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

/**
 *
 * @author dmicol
 */
public abstract class SHAEncoder {
    private SHAEncoder() {
    }
    
    public static String encode(String value) throws NoSuchAlgorithmException {
        MessageDigest digest = MessageDigest.getInstance("SHA-512"); 
        digest.update(value.getBytes()); 
        byte messageDigest[] = digest.digest();
        StringBuilder hexString = new StringBuilder();
        for (int i = 0; i < messageDigest.length; i++) {
            String h = Integer.toHexString(0xFF & messageDigest[i]);
            while (h.length() < 2) {
                h = "0" + h;
            }
            hexString.append(h);
        }
        return hexString.toString();
    }
}
