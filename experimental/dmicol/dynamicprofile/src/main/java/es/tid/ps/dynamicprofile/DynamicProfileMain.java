package es.tid.ps.dynamicprofile;

import es.tid.ps.dynamicprofile.dictionary.DictionaryHandler;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.IOException;

public class DynamicProfileMain {
    public static void main(String[] args) {
        String termsInDomainFlatFileName = args[0];
        String dictionaryFileName = args[1];
        String categoryPatterMappingFileName = args[2];
        String categoryNamesFileName = args[3];

        try {
            DictionaryHandler.init(termsInDomainFlatFileName,
                    dictionaryFileName, categoryPatterMappingFileName,
                    categoryNamesFileName);

            BufferedReader br = new BufferedReader(new InputStreamReader(
                    System.in));
            while (true) {
                String url = br.readLine();
                String categories = DictionaryHandler.getUrlCategories(url);
                System.out.println("Categories: " + categories);
            }
        } catch (IOException ex) {
            return;
        }
    }
}
