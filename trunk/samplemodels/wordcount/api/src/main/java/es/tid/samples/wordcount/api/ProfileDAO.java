package es.tid.samples.wordcount.api;

import com.mongodb.BasicDBObject;
import com.mongodb.DBCollection;
import com.mongodb.DBObject;
import javax.annotation.PostConstruct;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

/**
 *
 * @author sortega
 */
@Component
public class ProfileDAO {
    private static final String PROFILE_COLLECTION = "profile";
    private static final DBObject CATEGORIES_COL =
            new BasicDBObject("categories", true);
    private static final DBObject DATE_COL =
            new BasicDBObject("date", -1);
    @Autowired(required = true)
    private MongoService mongo;
    private DBCollection profiles;

    @PostConstruct
    public void init() {
        this.profiles = this.mongo.getDb().getCollection(PROFILE_COLLECTION);
    }

    public void setMongo(MongoService mongo) {
        this.mongo = mongo;
    }
}
