"""Top level urls. """

from django.conf.urls.defaults import patterns, include, url
from django.contrib import admin

admin.autodiscover()

urlpatterns = patterns('',
    url(r'^accounts/login/$', 'django.contrib.auth.views.login',
        { 'template_name': 'auth/user_login.html' }, name='login'),
    url(r'^accounts/logout/$', 'django.contrib.auth.views.logout',
        { 'next_page': '/' }, name='logout'),
    url(r'^admin/', include(admin.site.urls)),
    url(r'^', include('bdp_fe.jobconf.urls')),
)
