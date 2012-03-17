package avrosandbox;

public class IndividualProfile {
    public String userId;
    public String region;
    public CategoryInfo[] categories;

    @Override
    public String toString() {
        return "IndividualProfile{" + "userId=" + userId + ", categories=" + categories + '}';
    }
}
