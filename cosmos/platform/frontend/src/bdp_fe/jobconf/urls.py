"""Job configuration urls. """

from django.conf.urls.defaults import patterns, url

from bdp_fe.jobconf import views

urlpatterns = patterns('',
    url(r'^$', views.list_jobs, name="list_jobs"),
    url(r'new/$', views.new_job, name="new_job"),
    url(r'job/(\d+)/results/$', views.view_results, name="view_results"),
    url(r'job/(\d+)/config/$', views.config_job, name="config_job"),
    url(r'job/(\d+)/data/$', views.upload_data, name="upload_data"),
)
