'''
Created on 17/02/2012

@author: def
'''
from django.shortcuts import render_to_response
from django.template import RequestContext

def home(request):
    return render_to_response('wizard/home.html', {},context_instance=RequestContext(request))