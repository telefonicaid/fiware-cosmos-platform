from django.db import models

# Ingestion Model
class Ingestion(models.Model):
    name = models.CharField(max_length=100)
    streaming = models.BooleanField()
    file = models.BooleanField()
    path = models.CharField(max_length=200)
    estimated_size = models.IntegerField()
    
# Consumption Model
class Consumption(models.Model):
    outPut_folder = models.CharField(max_length=100)
    high_availability = models.BooleanField()
    data_exploination = models.BooleanField()