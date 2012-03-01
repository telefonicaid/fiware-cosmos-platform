from django.conf.urls.defaults import patterns, include, url
from Configuration.wizard.forms import IngestionForm, PreProcessingForm, WebProfilingForm, WizardForm

# Uncomment the next two lines to enable the admin:
from django.contrib import admin
admin.autodiscover()

urlpatterns = patterns('',
    # Examples:
    # url(r'^$', 'BackOffice.views.home', name='home'),
    # url(r'^BackOffice/', include('BackOffice.foo.urls')),

    # Uncomment the admin/doc line below to enable admin documentation:
    # url(r'^admin/doc/', include('django.contrib.admindocs.urls')),


    # Initial Page
    #url(r'^home/', 'wizard.views.views.home'),
    url(r'^monitoring/$', 'wizard.views.views.home', name='home'),
    # Uncomment the next line to enable the admin:
    #url(r'^admin/', include(admin.site.urls)),
    
    # Include Wizard views
    
    # FIXME : Review Regular Expressions and order
    
    url(r'^wizard/configuration/add/?$', WizardForm([IngestionForm, PreProcessingForm, WebProfilingForm, WebProfilingForm ])),        
    url(r'^wizard/configuration/?$','wizard.views.list_views.configuration'),        
    url(r'^wizard/configuration/(?P<pk>[a-z\d]+)/$', 'wizard.views.detail_views.configuration_detail'),
    
    url(r'^wizard/ingestion/add/', 'wizard.views.add_views.ingestion'),
    url(r'^wizard/ingestion/?$', 'wizard.views.list_views.ingestion'),
    url(r'^wizard/ingestion/(?P<pk>[a-z\d]+)/$', 'wizard.views.detail_views.ingestion_detail'),
    
    url(r'^wizard/preprocessing/add/', 'wizard.views.add_views.preprocessing'),
    url(r'^wizard/preprocessing/?$', 'wizard.views.list_views.preprocessing'),
    url(r'^wizard/preprocessing/(?P<pk>[a-z\d]+)/$', 'wizard.views.detail_views.preprocessing_detail'),
    
    url(r'^wizard/webprofiling/?$', 'wizard.views.list_views.webprofiling'),
    url(r'^wizard/webprofiling/add/', 'wizard.views.add_views.webprofiling'),
    url(r'^wizard/webprofiling/(?P<pk>[a-z\d]+)/$', 'wizard.views.detail_views.webprofiling_detail'),
)
