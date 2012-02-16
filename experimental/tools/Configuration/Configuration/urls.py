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
    url(r'^wizard/configuration/$', ConfigurationWizard([IngestionForm, PreProcessingForm, WebProfilingForm ])),    
    #url(r'^wizard/configuration/(?P<pk>[a-z\d]+)/$', 'wizard.views.detail'),
    url(r'^wizard/ingestion/(?P<pk>[a-z\d]+)/$', 'wizard.views.ingestion_detail'),
    url(r'^wizard/ingestion/?$', 'wizard.views.ingestion'),
    url(r'^wizard/preprocessing/(?P<pk>[a-z\d]+)/$', 'wizard.views.preprocessing_detail'),
    url(r'^wizard/preprocessing/?$', 'wizard.views.preprocessing'),
    url(r'^wizard/webprofiling/(?P<pk>[a-z\d]+)/$', 'wizard.views.webprofiling_detail'),
    url(r'^wizard/webprofiling/?$', 'wizard.views.webprofiling'),
)