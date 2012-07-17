package es.tid.smartsteps.footfalls.lookups;

import java.io.IOException;

import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.smartsteps.footfalls.data.generated.LookupProtocol.Lookup;

/**
 *
 * @author dmicol
 */
public class LookupRekeyerMapper extends Mapper<
        Text, TypedProtobufWritable<Lookup>,
        Text, TypedProtobufWritable<Lookup>> {

    public static enum RekeyBy {
        INVALID,
        KEY,
        VALUE
    }

    private RekeyBy rekeyBy;
    private Text outKey;

    @Override
    protected void setup(Context context) {
        this.rekeyBy = context.getConfiguration().getEnum(RekeyBy.class.getName(),
                                                          RekeyBy.INVALID);
        this.outKey = new Text();
    }

    @Override
    protected void map(Text key, TypedProtobufWritable<Lookup> value,
            Context context) throws IOException, InterruptedException {
        final Lookup lookup = value.get();
        switch (this.rekeyBy) {
            case KEY:
                this.outKey.set(lookup.getKey());
                break;
            case VALUE:
                this.outKey.set(lookup.getValue());
                break;
            default:
                throw new IllegalArgumentException("Invalid rekey by type");
        }
        context.write(this.outKey, value);
    }
}
