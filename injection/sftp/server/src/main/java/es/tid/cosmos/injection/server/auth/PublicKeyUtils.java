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

package es.tid.cosmos.injection.server.auth;

import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.math.BigInteger;
import java.nio.ByteBuffer;
import java.security.KeyFactory;
import java.security.NoSuchAlgorithmException;
import java.security.PublicKey;
import java.security.interfaces.DSAParams;
import java.security.interfaces.DSAPublicKey;
import java.security.interfaces.RSAPublicKey;
import java.security.spec.DSAPublicKeySpec;
import java.security.spec.InvalidKeySpecException;
import java.security.spec.RSAPublicKeySpec;

import org.apache.commons.codec.binary.Base64;

/**
 * Utils for serialize and deserialize PublicKey Java Objects
 * to linux authorized_keys entries like format
 *
 * @author ag453
 */
public final class PublicKeyUtils {

    /**
     * Index of the encoded field column.
     * Keys follow this pattern: "keyType encodedKey user@machine"
     */
    private static final int ENCODED_FIELD_COLUMN = 1;
    private static final String SSH_RSA = "ssh-rsa";
    private static final String SSH_DSA = "ssh-dss";
    private static final String RSA = "RSA";
    private static final String DSA = "DSA";

    private PublicKeyUtils() {}

    /**
     * Encode PublicKey (RSA or DSA encoding) Java Type to Base64 String
     * as linux authorized entries like format
     *
     * @param publicKey RSA or DSA type Public Key
     * @param user like "user@machine"
     * @return String as linux authorized_keys entries like format.
     * @throws IOException
     */
    public static String encodePublicKey(PublicKey publicKey, String user)
            throws IOException {
        PublicKeyEncoder pkEncoder = new PublicKeyEncoder(user, publicKey);
        return pkEncoder.getPublicKey();
    }

    /**
     * @param publicKey String as linux authorized_keys entries like format.
     * @return PublicKey Java Object containing the String serialized key
     *  used as argument.
     *
     * @throws NoSuchAlgorithmException
     * @throws InvalidKeySpecException
     */
    public static PublicKey decodePublicKey(String publicKey)
            throws NoSuchAlgorithmException, InvalidKeySpecException {
        String encodedKey = publicKey.trim().split(" ")[ENCODED_FIELD_COLUMN];
        byte[] binaryKeyArray = Base64.decodeBase64(encodedKey.getBytes());
        return new PublicKeyDecoder(binaryKeyArray).decode();
    }

    private static class PublicKeyEncoder {

        private PublicKey publicKey;
        private String user;

        public PublicKeyEncoder(String user, PublicKey publicKey) {
            this.publicKey = publicKey;
            this.user = user;
        }

        public String getPublicKey() throws IOException {
            if (this.publicKey.getAlgorithm().equals(RSA)) {
                return this.getRSAKey();
            }
            if (this.publicKey.getAlgorithm().equals(DSA)) {
                return this.getDSAKey();
            }
            throw new IllegalArgumentException("Unknown public key encoding: " +
                    this.publicKey.getAlgorithm());
        }

        /**
         * Encode String DSA Public Key encoded on authorized_hosts like format.
         *
         * @return String DSA Public Key encoded as authorized_hosts like format
         * as "keyType Base64EncodedKey user@host"
         * @throws IOException
         */
        private String getDSAKey() throws IOException {
            DSAPublicKey dsaPublicKey = (DSAPublicKey) this.publicKey;
            DSAParams dsaParams = dsaPublicKey.getParams();
            ByteArrayOutputStream byteOs = new ByteArrayOutputStream();
            DataOutputStream dos = new DataOutputStream(byteOs);
            dos.writeInt(SSH_DSA.getBytes().length);
            dos.write(SSH_DSA.getBytes());
            dos.writeInt(dsaParams.getP().toByteArray().length);
            dos.write(dsaParams.getP().toByteArray());
            dos.writeInt(dsaParams.getQ().toByteArray().length);
            dos.write(dsaParams.getQ().toByteArray());
            dos.writeInt(dsaParams.getG().toByteArray().length);
            dos.write(dsaParams.getG().toByteArray());
            dos.writeInt(dsaPublicKey.getY().toByteArray().length);
            dos.write(dsaPublicKey.getY().toByteArray());
            String publicKeyEncoded = new String(
                    Base64.encodeBase64(byteOs.toByteArray()));
            return String.format(
                    "%1$s %2$s %3$s", SSH_DSA, publicKeyEncoded, this.user);
        }

        /**
         * Encode String RSA Public Key encoded on authorized_hosts like format
         * as keyType Base64EncodedKey user@host
         *
         * @return String RSA Public Key encoded as authorized_hosts like format
         * as "keyType Base64EncodedKey user@host"
         * @throws IOException
         */
        private String getRSAKey() throws IOException {
            RSAPublicKey rsaPublicKey = (RSAPublicKey) this.publicKey;
            ByteArrayOutputStream byteOs = new ByteArrayOutputStream();
            DataOutputStream dos = new DataOutputStream(byteOs);
            dos.writeInt(SSH_RSA.getBytes().length);
            dos.write(SSH_RSA.getBytes());
            dos.writeInt(rsaPublicKey.getPublicExponent().toByteArray().length);
            dos.write(rsaPublicKey.getPublicExponent().toByteArray());
            dos.writeInt(rsaPublicKey.getModulus().toByteArray().length);
            dos.write(rsaPublicKey.getModulus().toByteArray());
            String publicKeyEncoded = new String(
                    Base64.encodeBase64(byteOs.toByteArray()));
            return String.format(
                    "%1$s %2$s %3$s", SSH_RSA, publicKeyEncoded, this.user);
        }
    }

    private static class PublicKeyDecoder {

        // ByteBuffer containing binary key and current position to read.
        private ByteBuffer bkey;
        private String type;

        public PublicKeyDecoder(byte[] binaryKey){
            this.bkey = ByteBuffer.allocate(binaryKey.length);
            this.bkey.put(binaryKey);
            this.bkey.position(0);
            this.type = this.decodeType();
        }

        public PublicKey decode()
                throws InvalidKeySpecException, NoSuchAlgorithmException {
            if (this.type.equals(SSH_RSA)) {
                return this.decodeRSAKey();
            }
            if (this.type.equals(SSH_DSA)) {
                return this.decodeDSAKey();
            }
            throw new IllegalArgumentException(
                    "Unknown public key encoding: " + this.type);
        }

        /**
         * Decode DSA String authorized_hosts file encoded like Public Key
         * as "keyType Base64EncodedKey user@host" on a byte[] and returns
         * PublicKey Java Object
         *
         * @return PublicKey Java Object containing DSA a key.
         * @throws NoSuchAlgorithmException
         * @throws InvalidKeySpecException
         */
        private PublicKey decodeDSAKey()
                throws NoSuchAlgorithmException, InvalidKeySpecException {
            BigInteger p = this.decodeBigEndianBigInt();
            BigInteger q = this.decodeBigEndianBigInt();
            BigInteger g = this.decodeBigEndianBigInt();
            BigInteger y = this.decodeBigEndianBigInt();
            DSAPublicKeySpec spec = new DSAPublicKeySpec(y, p, q, g);
            return KeyFactory.getInstance(DSA).generatePublic(spec);
        }

        /**
         * Decode RSA String authorized_hosts file encoded like Public Key
         * as "keyType Base64EncodedKey user@host" on a byte[] and returns
         * PublicKey Java Object
         *
         * @return PublicKey Java Object containing RSA a key.
         * @throws NoSuchAlgorithmException
         * @throws InvalidKeySpecException
         */
        private PublicKey decodeRSAKey()
                throws NoSuchAlgorithmException, InvalidKeySpecException {
            BigInteger e = this.decodeBigEndianBigInt();
            BigInteger m = this.decodeBigEndianBigInt();
            RSAPublicKeySpec spec = new RSAPublicKeySpec(m, e);
            return KeyFactory.getInstance(RSA).generatePublic(spec);
        }

        /**
         * Return a BigInteger value in big-endian order of byte array
         * and increments read position to next value readable on binary key
         * encoded byte array.
         *
         * @return BigInteger value in big-endian order of byte array.
         */
        private BigInteger decodeBigEndianBigInt() {
            int len = this.decodeBigEndianInt();
            byte[] bigInt = new byte[len];
            System.arraycopy(
                    this.bkey.array(), this.bkey.position(), bigInt, 0, len);
            this.bkey.position(this.bkey.position() + len);
            return new BigInteger(bigInt);
        }

        /**
         * Return a integer value in big-endian order of byte array and
         * increments read position to next value readable on binary key encoded
         * byte array.
         *
         * @return integer value in big-endian order of byte array.
         */
        private int decodeBigEndianInt() {
            int value = 0x00;
            for (int offset = 24 ; offset >= 0 ; offset = offset - 8) {
                value += (this.bkey.get() & 0xFF) << offset;
            }
            return value;
        }

        /**
         * Return String that represents the key encoded type (RSA, DSA, etc)
         * and increments read position to next value readable on binary key
         * encoded byte array.
         *
         * @return String that represents encoding of key
         * ("ssh-rsa" or "ssh-dss") as "keyType Base64EncodedKey user@host"
         */
        private String decodeType() {
            int len = this.decodeBigEndianInt();
            String type = new String(
                    this.bkey.array(),
                    this.bkey.position(),
                    len);
            this.bkey.position(this.bkey.position() + len);
            return type;
        }
    }
}
