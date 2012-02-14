from django.db import models
from djangotoolbox.fields import ListField
from djangotoolbox.fields import EmbeddedModelField
from django import forms

# Create your models here.
class StringListField(forms.CharField):
    def prepare_value(self, value):
        return ', '.join(value)

    def to_python(self, value):
        if not value:
            return []
        return [item.strip() for item in value.split(',')]
    
class LabelField(ListField):
    def formfield(self,**kwargs):
        return models.Field.formfield(self, StringListField,**kwargs)
            
# Create your models here.
class Template(models.Model):
    name = models.CharField(max_length=200)
    attributes = LabelField(EmbeddedModelField('Attribute'))

class Attribute(models.Model):
    identifier = models.CharField(max_length=200)
    type = models.CharField(max_length=20)
    label = EmbeddedModelField('Label')

class Label(models.Model):
    header = models.CharField(max_length=200)
    options = LabelField()