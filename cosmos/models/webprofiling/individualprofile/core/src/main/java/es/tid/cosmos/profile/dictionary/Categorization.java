package es.tid.cosmos.profile.dictionary;

/**
 * Class that represents the result of a categorization.
 * 
 * @author dmicol
 */
public class Categorization {
    private CategorizationResult result;
    private String[] categories;
    
    public Categorization() {
    }
    
    public CategorizationResult getResult() {
        return this.result;
    }
    
    public void setResult(CategorizationResult result) {
        this.result = result;
    }
    
    public String[] getCategories() {
        return this.categories;
    }
    
    public void setCategories(String[] categories) {
        this.categories = categories.clone();
    }
}
