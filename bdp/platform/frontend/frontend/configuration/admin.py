# Add configuration models to administration site
from django.contrib.admin import site
from BackOfficeUI.configuration.models import Template, Attribute, Label

site.register(Template)
site.register(Attribute)
site.register(Label)
