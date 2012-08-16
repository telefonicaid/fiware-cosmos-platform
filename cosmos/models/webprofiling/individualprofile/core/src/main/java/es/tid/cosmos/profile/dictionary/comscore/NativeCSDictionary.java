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

package es.tid.cosmos.profile.dictionary.comscore;

/**
 * Interface for the native ComScore dictionary.
 *
 * @author sortega
 */
public interface NativeCSDictionary {
    int DEFAULT_MODE = 1;

    /**
     * Applies the dictionary to the given URL, and returns the pattern ID for
     * such URL.
     *
     * @param szURL
     *          the url to apply the dictionary to
     * @return the pattern ID of the URL
     */
    long applyDictionaryUsingUrl(String szURL);

    /**
     * Initializes the dictionary wrapper using the terms in domain file.
     *
     * @param iMode
     *              the operation mode (value should be 1)
     * @param szTermsInDomainFlatFileName
     *              the terms in domain file name
     * @return whether the initialization succeeded
     */
    boolean initFromTermsInDomainFlatFile(int iMode,
            String szTermsInDomainFlatFileName);

    /**
     * Loads the comScore dictionary in memory.
     *
     * @param iMode
     *          the operation mode (value should be 1)
     * @param szTermsInDomainFlatFileName
     *          the terms in domain file name
     * @param szDictionaryName
     *          the file name of the dictionary
     * @return whether the load of the dictionary succeeded
     */
    boolean loadCSDictionary(int iMode, String szTermsInDomainFlatFileName,
            String szDictionaryName);
}
