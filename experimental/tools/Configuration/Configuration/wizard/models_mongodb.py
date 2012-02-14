from django.db import models
from djangotoolbox.fields import ListField
from djangotoolbox.fields import EmbeddedModelField
from django import forms
#from Configuration.wizard.forms import StringListField

# Create your models here.
class StringListField(forms.CharField):
    def prepare_value(self, value):
        return ', '.join(value)

    def to_python(self, value):
        if not value:
            return []
        return [item.strip() for item in value.split(',')]
    
class LabelField(models.Model):
    def formField(self,**kwargs):
        return models.Field.formfield(self, StringListField,**kwargs)
    
class Label(models.Model):
    header = models.CharField(max_length=200)
    options = ListField()
    
class Attribute(models.Model):
    identifier = models.CharField(max_length=200)
    type = models.CharField(max_length=20)
    label = EmbeddedModelField('Label')
    
class Template(models.Model):
    name = models.CharField(max_length=200)
    mores = models.BooleanField()
    attributes = ListField(EmbeddedModelField('Attribute'))

