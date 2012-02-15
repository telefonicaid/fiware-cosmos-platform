from django.conf.urls.defaults import patterns, include, url
from Configuration.wizard.forms import IngestionForm, PreProcessingForm, WebProfilingForm, ConfigurationWizard

# Uncomment the next two lines to enable the admin:
from django.contrib import admin
admin.autodiscover()

urlpatterns = patterns('',
    # Examples:
    # url(r'^$', 'BackOffice.views.home', name='home'),
    # url(r'^BackOffice/', include('BackOffice.foo.urls')),

    # Uncomment the admin/doc line below to enable admin documentation:
    # url(r'^admin/doc/', include('django.contrib.admindocs.urls')),

    # Uncomment the next line to enable the admin:
    url(r'^admin/', include(admin.site.urls)),
    
    # Include Wizard views
    url(r'^wizard/ingestion/$', 'wizard.views.ingestion'),
    url(r'^wizard/preProcessing/$', 'wizard.views.preProcessing'),
    #url(r'^wizard/configuration/$', 'wizard.views.configuration'),
    url(r'^wizard/configuration/$', ConfigurationWizard([IngestionForm,PreProcessingForm, WebProfilingForm ])),
    #url(r'^wizard/', include('Configuration.urls'))
)
