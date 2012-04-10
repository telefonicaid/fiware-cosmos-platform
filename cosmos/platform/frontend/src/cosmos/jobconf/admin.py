"""
Configuration for enabling jobs on the admin area.

"""

from django.contrib import admin

from cosmos.jobconf.models import Job, JobModel


class JobModelAdmin(admin.TabularInline):
    model = JobModel


class JobAdmin(admin.ModelAdmin):
    inlines = [
        JobModelAdmin,
    ]
    ordering = ('id', 'name',)


admin.site.register(Job, JobAdmin)
