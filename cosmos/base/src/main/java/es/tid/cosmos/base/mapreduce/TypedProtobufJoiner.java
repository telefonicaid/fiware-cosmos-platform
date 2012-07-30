package es.tid.cosmos.base.mapreduce;

import java.io.IOException;
import java.util.List;
import java.util.Map;

import com.google.protobuf.Message;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;

/**
 * Reducer that joins two types of typed protobuf writables.
 *
 * @author sortega
 */
public abstract class TypedProtobufJoiner<JoinKey, LeftVal extends Message,
        RightVal extends Message, OutKey, OutVal>
        extends Reducer<JoinKey, TypedProtobufWritable<Message>, OutKey, OutVal> {

    private final Class<? extends LeftVal> leftClass;
    private final Class<? extends RightVal> rightClass;

    /**
     * Types of the messages to join for reflection purposes.
     *
     * @param leftClass
     * @param rightClass
     */
    public TypedProtobufJoiner(Class<? extends LeftVal> leftClass,
                               Class<? extends RightVal> rightClass) {
        this.leftClass = leftClass;
        this.rightClass = rightClass;
    }

    @Override
    protected void reduce(JoinKey key,
                          Iterable<TypedProtobufWritable<Message>> values,
                          Context context) throws IOException, InterruptedException {
        Map<Class, List> dividedLists = TypedProtobufWritable.groupByClass(
                values, this.leftClass, this.rightClass);
        List<? extends LeftVal> leftList = dividedLists.get(this.leftClass);
        List<? extends RightVal> rightList = dividedLists.get(this.rightClass);
        this.join(key, leftList, rightList, context);
    }

    /**
     * Should be overridden to implement the join functionality.
     */
    protected abstract void join(JoinKey key, List<? extends LeftVal> leftList,
                                 List<? extends RightVal> rightList, Context context)
            throws IOException, InterruptedException;
}
