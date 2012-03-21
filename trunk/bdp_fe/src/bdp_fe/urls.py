"""Top level urls. """

from django.conf.urls.defaults import patterns, include, url
from django.contrib import admin

#admin.autodiscover()

urlpatterns = patterns('',
    url(r'^accounts/login/$', 'django.contrib.auth.views.login',
        { 'template_name': 'login.html' }),
    url(r'^accounts/logout/$', 'django.contrib.auth.views.logout',
        { 'next_page': '/' }),
    url(r'^admin/', include(admin.site.urls)),
    url(r'^', include('bdp_fe.jobconf.urls')),
)
