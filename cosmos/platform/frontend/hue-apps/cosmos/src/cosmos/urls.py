"""
URL mappings.

"""
from django.conf.urls.defaults import patterns, url

urlpatterns = patterns('cosmos',
  url(r'^$',                   'views.index',          name='list_jobs'),
  url(r'^jobs/run/$',          'views.define_job',     name='define_job'),
  url(r'^jobs/run/config/$',   'views.configure_job',  name='configure_job'),
  url(r'^jobs/run/confirm/$',  'views.confirm_job',    name='confirm_job'),
  url(r'^jobs/run/cancel/$',   'views.cancel_job',     name='cancel_job'),
  url(r'^upload/$',            'views.upload_index',   name='upload_index'),
  url(r'^results/$',           'views.list_results',   name='list_results'),
  url(r'^job/(\d+)/results/$', 'views.show_results',   name='show_results'),
)
