package es.tid.bdp.samples.wordcount;

import com.mongodb.BasicDBObject;
import com.mongodb.DBCollection;
import com.mongodb.DBCursor;
import com.mongodb.DBObject;
import javax.annotation.PostConstruct;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

/**
 *
 * @author dmicol, sortega
 */
@Component
public class WordCountDAO {
    private static final String WORD_COUNT_COLLECTION = "wordcount";
    private static final String WORD_COLUMN = "_id";
    private static final String COUNT_COLUMN = "value";
    
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
        BasicDBObject ref = new BasicDBObject(WORD_COLUMN, word);
        DBCursor results = this.wordCounts.find(ref);
        if (results.size() == 0) {
            // Word is not in the provided text
            return 0;
        } else if (results.size() > 1) {
            // More than one document for the same word. This shouldn't happen
            throw new IllegalStateException();
        } else {
            DBObject count = results.iterator().next();
            return (Long.parseLong(count.get(COUNT_COLUMN).toString()));
        } 
    }
}
