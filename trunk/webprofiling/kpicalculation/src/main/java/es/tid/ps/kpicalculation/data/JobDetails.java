package es.tid.ps.kpicalculation.data;

/**
 * @author javierb
 */
public class JobDetails {
    private String name;
    private String fields;
    private String group;

    public JobDetails(String name, String fields) {
        this(name, fields, null);
    }

    public JobDetails(String name, String fields, String group) {
        this.name = name;
        this.fields = fields;
        this.group = group;
    }

    public String getName() {
        return this.name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getFields() {
        return this.fields;
    }

    public void setFields(String fields) {
        this.fields = fields;
    }

    public String getGroup() {
        return this.group;
    }

    public void setGroup(String group) {
        this.group = group;
    }
}
