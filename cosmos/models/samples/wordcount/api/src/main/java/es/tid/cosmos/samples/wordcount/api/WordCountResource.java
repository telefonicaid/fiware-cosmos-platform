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
