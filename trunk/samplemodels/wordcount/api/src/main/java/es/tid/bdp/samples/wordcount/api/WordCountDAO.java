package es.tid.bdp.samples.wordcount.api;

import com.mongodb.BasicDBObject;
import com.mongodb.DBCollection;
import com.mongodb.DBCursor;
import com.mongodb.DBObject;
import javax.annotation.PostConstruct;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

/**
 *
 * @author sortega
 */
@Component
public class WordCountDAO {
    private static final String WORD_COUNT_COLLECTION = "wordcount";
    
    @Autowired(required = true)
    private MongoService mongo;
    private DBCollection wordCounts;

    @PostConstruct
    public void init() {
        this.wordCounts = this.mongo.getDb().getCollection(
                WORD_COUNT_COLLECTION);
    }

    public void setMongo(MongoService mongo) {
        this.mongo = mongo;
    }
    
    public long getCount(String word) {
        BasicDBObject ref = new BasicDBObject("id", word);
        DBCursor result = this.wordCounts.find(ref);
        return result.size();
    }
}
