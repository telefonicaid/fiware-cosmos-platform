from django.conf.urls.defaults import patterns, include, url

# To include Wizard
from django.conf.urls.defaults import *
from BackOffice.wizard.forms import IngestionForm, ConsumptionForm, ConfigurationWizard

# Uncomment the next two lines to enable the admin:
from django.contrib import admin
admin.autodiscover()

# DataSets Views
urlpatterns = patterns('wizard.views',

    # dataSetInputs Views
    url(r'^$', 'ingestion'),
        # dataSetInputs Views
    url(r'^ingestion/$', 'ingestion'),
    
    # Wizard
    url(r'^configuration/$', ConfigurationWizard([IngestionForm,ConsumptionForm])), 
)

