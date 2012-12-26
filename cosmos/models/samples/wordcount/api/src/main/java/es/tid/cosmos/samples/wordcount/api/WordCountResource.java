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

package es.tid.cosmos.samples.wordcount.api;

import javax.ws.rs.GET;
import javax.ws.rs.Path;
import javax.ws.rs.PathParam;
import javax.ws.rs.Produces;
import javax.ws.rs.core.MediaType;

import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;
import org.springframework.beans.factory.annotation.Autowired;

/**
 *
 * @author dmicol, sortega
 */
@Path("/wordcount/{word}")
@Component
@Scope("request")
public class WordCountResource {
    @Autowired(required = true)
    private WordCountDAO wordCount;

    public void setDao(WordCountDAO dao) {
        this.wordCount = dao;
    }

    @GET
    @Produces(MediaType.APPLICATION_JSON)
    public long getCount(@PathParam("word") String word) {
        return this.wordCount.getCount(word);
    }
}
