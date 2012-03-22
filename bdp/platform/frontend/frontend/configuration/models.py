from django.db import models

# Create your models here.
class Template(models.Model):
    name = models.CharField(max_length=200)
    attributes = models.ForeignKey('Attribute')

class Attribute(models.Model):
    identifier = models.CharField(max_length=200)
    type = models.CharField(max_length=20)
    label = models.ForeignKey('Label')

class Label(models.Model):
    header = models.CharField(max_length=200)
    options = models.TextField()