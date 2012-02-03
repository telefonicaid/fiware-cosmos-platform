package es.tid.test.profile;

import com.mongodb.DB;
import com.mongodb.Mongo;
import java.net.UnknownHostException;
import javax.annotation.PreDestroy;
import org.springframework.stereotype.Component;

/**
 *
 * @author sortega
 */
@Component
public class MongoService {
    private final Mongo mongo;
    private final DB db;

    public MongoService(String host, String database) throws
            UnknownHostException {
        this.mongo = new Mongo(host);
        this.db = this.mongo.getDB(database);
    }

    public DB getDb() {
        return db;
    }

    public Mongo getMongo() {
        return mongo;
    }

    @PreDestroy
    public void destroy() {
        this.mongo.close();
    }
}
