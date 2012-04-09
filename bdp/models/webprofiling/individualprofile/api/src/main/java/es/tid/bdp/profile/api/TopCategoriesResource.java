package es.tid.bdp.profile.api;

import java.util.List;

import com.sun.jersey.api.NotFoundException;
import javax.ws.rs.GET;
import javax.ws.rs.Path;
import javax.ws.rs.PathParam;
import javax.ws.rs.Produces;
import javax.ws.rs.core.MediaType;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

/**
 *
 * @author sortega
 */
@Path("/{username}/top/{n}")
@Component
@Scope("request")
public class TopCategoriesResource {
    @Autowired(required = true)
    private ProfileDAO profile;

    public void setDao(ProfileDAO dao) {
        this.profile = dao;
    }

    @GET
    @Produces(MediaType.APPLICATION_JSON)
    public List getTop(@PathParam("username") String userName,
                       @PathParam("n") int n) {
        try {
            return Categories.getTop(this.profile, userName, n);
        } catch (IllegalArgumentException ex) {
            throw new NotFoundException(ex.getMessage());
        }
    }
}
