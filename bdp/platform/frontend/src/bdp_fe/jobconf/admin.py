"""
Configuration for enabling jobs on the admin area.

"""

from django.contrib import admin

from bdp_fe.jobconf.models import Job, JobModel


class JobModelAdmin(admin.TabularInline):
    model = JobModel


class JobAdmin(admin.ModelAdmin):
    inlines = [
        JobModelAdmin,
    ]
    ordering = ('id', 'name',)


admin.site.register(Job, JobAdmin)
