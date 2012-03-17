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
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
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
    public List getTop(@PathParam("username") String userName,
                         @PathParam("n") int n) {
        if (n < 1) {
            throw new NotFoundException();
        }
        final CategoryMap categoryMap = profile.getLastCategoryMap(userName);
        List topN = new LinkedList();
        for (final String category: categoryMap.getTop(n)) {
            topN.add(new HashMap() {{
                put("category", category);
                put("count", categoryMap.get(category));
            }});
        }
        return topN;
    }
}
