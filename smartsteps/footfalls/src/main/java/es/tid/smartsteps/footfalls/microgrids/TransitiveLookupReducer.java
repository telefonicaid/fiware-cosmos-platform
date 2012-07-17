package es.tid.smartsteps.footfalls.microgrids;

import java.io.IOException;
import java.util.List;
import java.util.LinkedList;

import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.smartsteps.footfalls.microgrids.data.generated.LookupProtocol.Lookup;

/**
 *
 * @author dmicol
 */
class TransitiveLookupReducer extends Reducer<
        Text, TypedProtobufWritable<Lookup>,
        Text, TypedProtobufWritable<Lookup>> {

    private TypedProtobufWritable<Lookup> outValue;

    @Override
    protected void setup(Context context) {
        this.outValue = new TypedProtobufWritable<Lookup>();
    }

    @Override
    protected void reduce(Text key,
            Iterable<TypedProtobufWritable<Lookup>> values, Context context)
            throws IOException, InterruptedException {
        final String strKey = key.toString();
        List<Lookup> sourceLookups = new LinkedList<Lookup>();
        List<Lookup> targetLookups = new LinkedList<Lookup>();
        for (TypedProtobufWritable<Lookup> value : values) {
            final Lookup lookup = value.get();
            if (strKey.equals(lookup.getKey())) {
                sourceLookups.add(lookup);
            } else if (strKey.equals(lookup.getValue())) {
                targetLookups.add(lookup);
            }
        }
        
        for (Lookup sourceLookup : sourceLookups) {
            for (Lookup targetLookup : targetLookups) {
                Lookup.Builder transitiveLookup = Lookup.newBuilder();
                transitiveLookup.setKey(sourceLookup.getKey());
                transitiveLookup.setValue(targetLookup.getKey());
                transitiveLookup.setProportion(sourceLookup.getProportion()
                                               * targetLookup.getProportion());
                this.outValue.set(transitiveLookup.build());
                context.write(key, this.outValue);
            }
        }
    }
}
