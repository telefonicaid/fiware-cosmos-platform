from django.conf.urls.defaults import patterns, url

# DataSets Views
urlpatterns = patterns('wizard.views',
    url(r'^configuration/add/?$','add_views.configuration'),                 
    url(r'^configuration/?$','list_views.configuration'),        
    url(r'^configuration/(?P<pk>[a-z\d]+)/$', 'detail_views.configuration_detail'),
    
    url(r'^ingestion/add/', 'add_views.ingestion'),
    url(r'^ingestion/?$', 'list_views.ingestion'),
    url(r'^ingestion/(?P<pk>[a-z\d]+)/$', 'detail_views.ingestion_detail'),
    
    url(r'^preprocessing/add/', 'add_views.preprocessing'),
    url(r'^preprocessing/?$', 'list_views.preprocessing'),
    url(r'^preprocessing/(?P<pk>[a-z\d]+)/$', 'detail_views.preprocessing_detail'),
    
    url(r'^webprofiling/?$', 'list_views.webprofiling'),
    url(r'^webprofiling/add/', 'add_views.webprofiling'),
    url(r'^webprofiling/(?P<pk>[a-z\d]+)/$', 'detail_views.webprofiling_detail'),
)

