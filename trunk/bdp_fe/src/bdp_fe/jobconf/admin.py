"""
Configuration for enabling jobs on the admin area.

"""

from django.contrib import admin

from models import Job, JobModel

class JobModelAdmin(admin.TabularInline):
    model = JobModel

class JobAdmin(admin.ModelAdmin):
    inlines = [
        JobModelAdmin,
    ]
    ordering = ('name',)

admin.site.register(Job, JobAdmin)
