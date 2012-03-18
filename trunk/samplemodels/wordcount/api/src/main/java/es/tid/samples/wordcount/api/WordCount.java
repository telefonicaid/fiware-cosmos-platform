package es.tid.samples.wordcount.api;

import java.util.List;

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
@Path("/{username}/wordcount/{n}")
@Component
@Scope("request")
public class WordCount {
    @Autowired(required = true)
    private ProfileDAO profile;

    public void setDao(ProfileDAO dao) {
        this.profile = dao;
    }

    @GET
    @Produces(MediaType.APPLICATION_JSON)
    public List getTop(@PathParam("username") String userName,
                       @PathParam("word") String word) {
        // TODO
        return null;
    }
}
