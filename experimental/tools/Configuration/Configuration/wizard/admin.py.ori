from Configuration.wizard.models import Template, Label, Attribute
from django.contrib.admin import site, ModelAdmin

def labels(instance):
    return ', '.join(instance.labels)

class TemplateAdmin(ModelAdmin):
    list_display = ['name', labels]

site.register(Template,TemplateAdmin)
site.register(Label)
site.register(Attribute)
