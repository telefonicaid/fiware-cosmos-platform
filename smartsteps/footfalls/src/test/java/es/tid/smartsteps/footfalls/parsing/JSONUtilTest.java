package es.tid.smartsteps.footfalls.parsing;

import net.sf.json.JSONArray;
import net.sf.json.JSONObject;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import org.junit.Before;
import org.junit.Test;

/**
 *
 * @author sortega
 */
public class JSONUtilTest {

    private JSONObject jsonObject;

    @Before
    public void setUp() {
        this.jsonObject = new JSONObject();
        this.jsonObject.put("prop1", 1);
        this.jsonObject.put("prop2", "val2");

        JSONArray nestedArray = new JSONArray();
        nestedArray.add("a");
        nestedArray.add("b");

        JSONObject nestedObject = new JSONObject();
        nestedObject.put("subprop1", "val3");
        nestedObject.put("subprop2", nestedArray);
        this.jsonObject.put("nest", nestedObject);
    }

    @Test
    public void shouldAccessPropertyName() throws Exception {
        assertEquals(1, JSONUtil.getProperty(this.jsonObject, "prop1"));
    }

    @Test
    public void shouldAccessNestedProperty() throws Exception {
        assertEquals("val3",
                     JSONUtil.getProperty(this.jsonObject, "nest.subprop1"));
        assertEquals("b",
                     JSONUtil.getProperty(this.jsonObject, "nest.subprop2.1"));
    }

    @Test
    public void shouldReturnNullWhenNotExists() throws Exception {
        assertNull(JSONUtil.getProperty(this.jsonObject, "nest.non-existant"));
        assertNull(JSONUtil.getProperty(this.jsonObject, "nest.0.not"));
    }

    @Test(expected = IllegalArgumentException.class)
    public void shouldThrowOnGettingEmptyPropertyName() throws Exception {
        JSONUtil.getProperty(this.jsonObject, "");
    }

    @Test(expected = IllegalArgumentException.class)
    public void shouldThrowWhenNestingOnWrongType() throws Exception {
        JSONUtil.getProperty(this.jsonObject, "prop1.wrong-subprop");
    }

    @Test
    public void shouldSetPropertyByName() throws Exception {
        JSONUtil.setProperty(this.jsonObject, "prop3", "value");
        assertEquals("value", this.jsonObject.get("prop3"));
    }

    @Test
    public void shouldSetNestedProperty() throws Exception {
        JSONArray value = new JSONArray();
        JSONUtil.setProperty(this.jsonObject, "nest.subprop2.3", value);
        JSONObject nest1 = (JSONObject) this.jsonObject.get("nest");
        JSONArray nest2 = (JSONArray) nest1.get("subprop2");
        JSONArray nest3 = (JSONArray) nest2.get(3);
        assertNotNull(nest3);

        JSONUtil.setProperty(this.jsonObject, "nest.subprop2.3.0", 42);
        assertEquals(42, JSONUtil.getProperty(this.jsonObject,
                                              "nest.subprop2.3.0"));
    }

    @Test(expected = IllegalArgumentException.class)
    public void shouldThrowOnSettingEmptyPropertyName() throws Exception {
        JSONUtil.setProperty(this.jsonObject, "", "value");
    }

    @Test(expected = IllegalArgumentException.class)
    public void shouldThrowOnInvalidJSONElement() throws Exception {
        JSONUtil.setProperty(null, "prop1.prop2", "value");
    }

    @Test(expected = IllegalArgumentException.class)
    public void shouldThrowOnInvalidNest() throws Exception {
        JSONUtil.setProperty(this.jsonObject, "prop1.subprop", "value");
    }
}
