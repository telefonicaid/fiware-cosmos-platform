package es.tid.test.profile.rest;

import com.sun.jersey.api.NotFoundException;
import es.tid.test.profile.CategoryMap;
import javax.ws.rs.GET;
import javax.ws.rs.Path;
import javax.ws.rs.PathParam;
import javax.ws.rs.Produces;
import javax.ws.rs.core.MediaType;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import es.tid.test.profile.ProfileDAO;
import es.tid.test.profile.StringUtil;
import java.util.List;
import org.springframework.beans.factory.annotation.Autowired;

/**
 *
 * @author sortega
 */
@Path("/{username}/top/{n}")
@Component
@Scope("request")
public class TopCategories {
    @Autowired(required = true)
    private ProfileDAO profile;

    public void setDao(ProfileDAO dao) {
        this.profile = dao;
    }

    @GET
    @Produces(MediaType.APPLICATION_JSON)
    public String getTop(@PathParam("username") String userName,
                         @PathParam("n") int n) {
        if (n < 1) {
            throw new NotFoundException();
        }
        CategoryMap categoryMap = profile.getLastCategoryMap(userName);
        final List<String> topN = categoryMap.getTop(n);
        if (topN.isEmpty()) {
            return "[]";
        } else {
            return "[\"" + StringUtil.join("\", \"", topN) + "\"]";
        }
    }
}
