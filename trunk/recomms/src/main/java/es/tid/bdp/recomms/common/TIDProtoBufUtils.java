package es.tid.bdp.recomms.common;

import java.util.Iterator;
import java.util.List;

import org.apache.mahout.math.Vector;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.bdp.recomms.data.RecommsProtocol.PrefandSimil;
import es.tid.bdp.recomms.data.RecommsProtocol.Preference;
import es.tid.bdp.recomms.data.RecommsProtocol.UserId;
import es.tid.bdp.recomms.data.RecommsProtocol.UserPref;
import es.tid.bdp.recomms.data.RecommsProtocol.VectorAndPrefs;
import es.tid.bdp.recomms.data.RecommsProtocol.VectorOrPrefs;
import es.tid.bdp.recomms.data.RecommsProtocol.VectorElement;

public class TIDProtoBufUtils {

    public static ProtobufWritable<VectorAndPrefs> ProtoBufVandP(
            Vector initVector, List<Long> userIds, List<Float> preferences) {
        ProtobufWritable<VectorAndPrefs> protoBufVandP = new ProtobufWritable<VectorAndPrefs>();
        protoBufVandP.setConverter(VectorAndPrefs.class);
        Iterator<Vector.Element> vIt = initVector.iterateNonZero();
        VectorAndPrefs.Builder newVector = VectorAndPrefs.newBuilder();
        while (vIt.hasNext()) {
            Vector.Element it = vIt.next();
            VectorElement.Builder vElem = VectorElement.newBuilder();
            vElem.setIndex(it.index()).setValue(it.get());
            newVector.addVector(vElem.build());
        }

        for (long uId : userIds) {
            UserId.Builder vId = UserId.newBuilder();
            vId.setId(uId);
            newVector.addUserIds(vId.build());
        }

        for (float pref : preferences) {
            Preference.Builder vPref = Preference.newBuilder();
            vPref.setPref(pref);
            newVector.addPrefs(vPref.build());
        }

        protoBufVandP.set(newVector.build());
        return protoBufVandP;
    }

    public static ProtobufWritable<VectorOrPrefs> ProtoBufVorP(long user,
            float preference) {
        ProtobufWritable<VectorOrPrefs> protoBufVorP = new ProtobufWritable<VectorOrPrefs>();
        protoBufVorP.setConverter(VectorOrPrefs.class);
        VectorOrPrefs.Builder newVector = VectorOrPrefs.newBuilder();
        newVector.setUserID(user).setValue(preference);
        protoBufVorP.set(newVector.build());
        return protoBufVorP;
    }

    public static ProtobufWritable<VectorOrPrefs> ProtoBufVorP(
            Vector similarities) {
        ProtobufWritable<VectorOrPrefs> protoBufVorP = new ProtobufWritable<VectorOrPrefs>();
        protoBufVorP.setConverter(VectorOrPrefs.class);
        Iterator<Vector.Element> vIt = similarities.iterateNonZero();
        VectorOrPrefs.Builder newVector = VectorOrPrefs.newBuilder();
        while (vIt.hasNext()) {
            Vector.Element it = vIt.next();
            VectorElement.Builder vElem = VectorElement.newBuilder();
            vElem.setIndex(it.index()).setValue(it.get());
            newVector.addVector(vElem);

        }
        protoBufVorP.set(newVector.build());
        return protoBufVorP;
    }

    public static ProtobufWritable<UserPref> ProtoBufuserPref(long userId,
            int itemId, float preference, int evenType) {
        ProtobufWritable<UserPref> protoBufUserPref = new ProtobufWritable<UserPref>();
        protoBufUserPref.setConverter(UserPref.class);

        UserPref.Builder uP = UserPref.newBuilder();

        uP.setUserID(userId).setItemId(itemId).setEventValue(preference)
                .setDeviceID("").setEventDate("").setEventType(evenType);

        protoBufUserPref.set(uP.build());
        return protoBufUserPref;
    }

    public static ProtobufWritable<PrefandSimil> ProtoBufPrefandSimil(
            float pref, Vector similarities) {
        ProtobufWritable<PrefandSimil> prefAndsimil = new ProtobufWritable<PrefandSimil>();
        prefAndsimil.setConverter(PrefandSimil.class);
        PrefandSimil.Builder newVector = PrefandSimil.newBuilder();

        newVector.setPrefvalue(pref);
        Iterator<Vector.Element> vIt = similarities.iterateNonZero();
        while (vIt.hasNext()) {
            Vector.Element it = vIt.next();
            VectorElement.Builder vElem = VectorElement.newBuilder();
            vElem.setIndex(it.index()).setValue(it.get());
            newVector.addVector(vElem);

        }
        prefAndsimil.set(newVector.build());

        return prefAndsimil;
    }
}