package es.tid.ps.profile.categoryextraction;

import java.util.ArrayList;
import java.util.List;

/**
 * @author sortega
 */
public class CategoryInformationFactory {

    private CategoryInformationFactory() {
    }

    public static void set(CategoryInformation categoryInformation,
                           String userId, String url, String date,
                           long count, String[] categoryNames) {
        categoryInformation.setUserId(userId);
        categoryInformation.setUrl(url);
        categoryInformation.setDate(date);
        categoryInformation.setCount(count);
        List<CharSequence> names = new ArrayList<CharSequence>();
        for (CharSequence name : categoryNames) {
            names.add(name.toString());
        }
        categoryInformation.setCategoryNames(names);
    }
}
