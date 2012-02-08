package es.tid.ps.profile.categoryextraction;

import java.util.Arrays;
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
        categoryInformation.setCategoryNames((List<CharSequence>) Arrays.asList(categoryNames));
    }
}
