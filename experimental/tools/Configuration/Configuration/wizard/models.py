from django.db import models
from djangotoolbox.fields import ListField, EmbeddedModelField, DictField
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
    template = models.CharField(max_length=200)
    attribute_values = ListField(EmbeddedModelField('Label'))
    def __unicode__(self):
        return u'%s' % (self.template)

class Attribute(models.Model):
    identifier = models.CharField(max_length=200)
    #type = models.CharField(max_length=20)
    def __unicode__(self):
        return u'%s' % (self.identifier)
    label = EmbeddedModelField('Label')

class Label(models.Model):
    header = models.CharField(max_length=200)
    options = DictField() #LabelField()
    def __unicode__(self):
        return u'%s, %s' % (self.header, self.options)
    
