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

package es.tid.cosmos.injection.sftp.auth;

import java.security.PublicKey;

import org.junit.Test;

import static junit.framework.Assert.assertEquals;

/**
 * RSAKeyUtilsTest
 *
 * @author ag453
 */
public class PublicKeyUtilsTest {

    private static final String RSAKEY = "ssh-rsa AAAAB3NzaC1yc2EAAAADAQAB" +
            "AAABAQCjYqwaYXrkzjvpWMOrXSfiXOvYTVPubefL/n2ev1yx0ZYzMgwNWc6AL3R" +
            "VZtXL6xyHpKUaDhd7ThJU3nLB8ay39VtUULQF6RJsfPt774ZLGH4QGxmiPiqptn" +
            "Cym1j2wPiK3pM4fvdV8DcOCiqEa8j0IkRXHkbMquA0GPyjEwk1NQtc30MfSyAID" +
            "YP/N+n0G3MAvrojA4gJyMKGq7nGFsZwvrlHyg70uMSdM6F50le7AI7uIc0TXGb8" +
            "4g5k8CGY3Fgmvr+WakuUngf52DLDxzluxP/ZjzMYbSzoE6vxrBa4u3RIaDNMY8X" +
            "jBFJ/F1ttmzJISdudYOELSjf1+tXxsxXr user@machine";
    private static final String DSAKEY = "ssh-dss AAAAB3NzaC1kc3MAAACBALMc" +
            "s56igEpksJKl8hQQoKB7/SkZq+lIoUREsqxJ5VRZ+eNCrC8e2R8v2+616UbGOko" +
            "azPPKWk8B4YTgp6FZkMX8zYP45W4j5J9zaThHqYKGPbL1KULTP8f527+QOMjXPT" +
            "cYb1yw6gpL6ckyRJr04I+5Ol+UK8Q2C5I3WSheFMkbAAAAFQCgUZTMvjJF0aJqG" +
            "SDmmkFrLCg6TwAAAIEAlpBRE/Ythud8ckTziSDi+pKHVwgqOEdkbT3qn9gUppzO" +
            "/RoCwZudic1+ug0mYz67UVgkxCcHF6lAgmaAFm/ZihMygsDqATFCa34eE8YiV9o" +
            "BxeQRSvtCLKpcf5XvU7+OUyPjJZCQKrXvXkf0N64+8rQ+BHfoiMOlteQYV29NmK" +
            "IAAACBAITJkz38mp964FG4fDb81g2/YQ9vBYX8CZZqooFSaMTUtoJo6deicrLwh" +
            "ct/nnPm5I6yowhyhkAff2Zr43DjQ3Q1s3J86CsfymPs6iTtlv1FioiY5RTMN4dW" +
            "O34qKWjd3bqM+L2Lt/BNpAxjmoaFYbnBs9lN1cfVwbABm7as8wEA user@machine";

    @Test
    public void encodeAndDecodeRSATest() throws Exception {
        PublicKey pk = PublicKeyUtils.decodePublicKey(this.RSAKEY);
        String encodedKey = PublicKeyUtils.encodePublicKey(pk, "user@machine");
        assertEquals(this.RSAKEY, encodedKey);
    }

    @Test
    public void encodeAndDecodeDSATest() throws Exception {
        PublicKey pk = PublicKeyUtils.decodePublicKey(this.DSAKEY);
        String encodedKey = PublicKeyUtils.encodePublicKey(pk, "user@machine");
        assertEquals(this.DSAKEY, encodedKey);
    }
}
