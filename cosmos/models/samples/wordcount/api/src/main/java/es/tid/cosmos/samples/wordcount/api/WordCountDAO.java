package es.tid.cosmos.samples.wordcount.api;

import java.util.List;
import javax.annotation.PostConstruct;

import com.mongodb.BasicDBObject;
import com.mongodb.DBCollection;
import com.mongodb.DBObject;
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
        return this.processResults(this.wordCounts.find(ref).toArray());
    }
    
    protected long processResults(List<DBObject> results) {
        if (results.isEmpty()) {
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
