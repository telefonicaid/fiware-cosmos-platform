package es.tid.cosmos.profile.api;

import java.util.Arrays;
import java.util.HashMap;
import java.util.List;

import com.sun.jersey.api.NotFoundException;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

/**
 * @author sortega
 */
public class AllCategoriesResourceTest {
    private AllCategoriesResource instance;
    private ProfileDAO profileMock;

    @Before
    public void setUp() throws Exception {
        this.instance = new AllCategoriesResource();
        this.profileMock = mock(ProfileDAO.class);
        this.instance.setDao(this.profileMock);
    }

    @Test
    public void shouldGetAllCategoriesInOrder() {
        when(this.profileMock.getLastCategoryMap("user1"))
                .thenReturn(new CategoryMap() {{
                    put("SPORTS", 1000L);
                    put("FASHION", 100L);
                    put("COMICS", 50L);
                    put("NEWS", 800L);
                }});

        String username = "user1";
        List expectedTopN = Arrays.asList(
                new HashMap() {{
                    put("category", "SPORTS");
                    put("count", 1000L);
                }},
                new HashMap() {{
                    put("category", "NEWS");
                    put("count", 800L);
                }},
                new HashMap() {{
                    put("category", "FASHION");
                    put("count", 100L);
                }},
                new HashMap() {{
                    put("category", "COMICS");
                    put("count", 50L);
                }});
        assertEquals(expectedTopN, this.instance.getAll(username));
    }

    @Test(expected=NotFoundException.class)
    public void shouldThrowExceptionOnUnknownUser() {
        when(this.profileMock.getLastCategoryMap("unknownUser"))
                .thenThrow(new IllegalArgumentException());
        this.instance.getAll("unknownUser");
    }
}
