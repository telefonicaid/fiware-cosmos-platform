# Create your views here.
from Configuration.wizard.models import Template, Configuration
from django.shortcuts import render_to_response
from django.views.decorators.csrf import csrf_protect
from django.template import RequestContext

@csrf_protect
def ingestion(request):
    templates = Template.objects.filter(template = 'IngestionTemplate')
    type = {}
    type['templateType'] = 'Ingestion'
    return render_to_response('wizard/template_list.html', {
                                                    'templates': templates, 'type' : type, 
                                                    },context_instance=RequestContext(request))
    
@csrf_protect
def preprocessing(request):
    templates = Template.objects.filter(template = 'PreProcessingTemplate')
    type = {}
    type['templateType'] = 'PreProcessing'
    return render_to_response('wizard/template_list.html', {
                                                    'templates': templates, 'type' : type, 
                                                    },context_instance=RequestContext(request))
    
@csrf_protect
def webprofiling(request):
    templates = Template.objects.filter(template = 'WebProfilingTemplate')
    type = {}
    type['templateType'] = 'WebProfiling'
    return render_to_response('wizard/template_list.html', {
                                                    'templates': templates, 'type' : type,
                                                    },context_instance=RequestContext(request))    

@csrf_protect
def configuration(request):
    configs = Configuration.objects.all()
    return render_to_response('wizard/configuration_list.html', {
                                                    'configs': configs, 
                                                    },context_instance=RequestContext(request))
    
