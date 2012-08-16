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

package es.tid.cosmos.profile.dictionary;

/**
 * Class that represents the result of a categorization.
 *
 * @author dmicol
 */
public class Categorization {
    private CategorizationResult result;
    private String[] categories;

    public Categorization() {
    }

    public CategorizationResult getResult() {
        return this.result;
    }

    public void setResult(CategorizationResult result) {
        this.result = result;
    }

    public String[] getCategories() {
        return this.categories;
    }

    public void setCategories(String[] categories) {
        this.categories = categories.clone();
    }
}
