"""Job configuration urls. """

from django.conf.urls.defaults import patterns, include, url

import views

urlpatterns = patterns('',
    url(r'^$', views.list_jobs),
    url(r'new/$', views.new_job),
    url(r'job/(\d+)/results/$', views.results),
)
