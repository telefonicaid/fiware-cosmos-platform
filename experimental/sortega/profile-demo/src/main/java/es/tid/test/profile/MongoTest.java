package es.tid.test.profile;

import com.mongodb.*;
import java.net.UnknownHostException;

/**
 *
 * @author sortega
 */
public class MongoTest {
    
    public static void main(String argv[]) throws UnknownHostException {
        Mongo mongo = new Mongo("localhost");
        DB db = mongo.getDB("individualprofile");
        DBCollection profiles = db.getCollection("profile");
        
        group(profiles);
        
        BasicDBObject query = new BasicDBObject();
        query.put("id", "Z4455");
        DBCursor results = profiles.find(query);
        for (DBObject result: results) {
            System.out.println(result);
            DBObject categories = (DBObject) result.get("categories");
            System.out.println(categories.get("Sport").getClass());
        }
    }

    private static void group(DBCollection profiles) throws MongoException {
        BasicDBObject key = new BasicDBObject("id", true);
        BasicDBObject initial = new BasicDBObject();
        initial.put("Sport", 0);
        DBObject group = profiles.group(key, null, initial, 
                "function(obj, accum) { "
                + "for (var key in obj.categories) { "
                + "   if (accum.hasOwnProperty(key)) {"
                + "       accum[key] += obj.categories[key];"
                + "   } else {"
                + "       accum[key] = obj.categories[key];"
                + "   }"
                + "}"
                + "}");
        System.out.println(group);
    }
}
