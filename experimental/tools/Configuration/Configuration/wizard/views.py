# Create your views here.
from Configuration.wizard.forms import IngestionForm, PreProcessingForm, WebProfilingForm
from Configuration.wizard.models import Template, Configuration
from django.shortcuts import render_to_response
from django.views.decorators.csrf import csrf_protect
from django.template import RequestContext

@csrf_protect
def ingestion(request):
    templates = Template.objects.filter(template = 'IngestionTemplate')
    return render_to_response('wizard/ingestion_list.html', {
                                                    'templates': templates, 
                                                    },context_instance=RequestContext(request))
    
@csrf_protect
def preprocessing(request):
    templates = Template.objects.filter(template = 'PreProcessingTemplate')
    return render_to_response('wizard/ingestion_list.html', {
                                                    'templates': templates, 
                                                    },context_instance=RequestContext(request))
    
@csrf_protect
def webprofiling(request):
    templates = Template.objects.filter(template = 'WebProfilingTemplate')
    return render_to_response('wizard/ingestion_list.html', {
                                                    'templates': templates, 
                                                    },context_instance=RequestContext(request))    


@csrf_protect
def configuration_detail(request,pk):
    config = Configuration.objects.get(id=pk)
    if config:
        templates = config.templates
                
        #form = IngestionForm(default_values)
        return render_to_response('wizard/ingestion.html', {
        #                                            'form': form, 
                                                    },context_instance=RequestContext(request))   
    else:
        return 'error.html'
    
@csrf_protect
def ingestion_detail(request,pk):
    template = Template.objects.get(id=pk)
    if template:
        attributes = template.attribute_values
        for label in attributes:
            for key in label.attribute:
                if key == 'IngestionMode':
                    choices = label.attribute[key]
                    if choices['Streaming'] == 1: 
                        select_mode = 'Streaming' 
                    else: select_mode = 'File'
                elif key == 'IngestionSize':
                    size = label.attribute[key]
                elif key == 'IngestionAddress':
                    path = label.attribute[key]
                
        default_values = {'path' : path, 'size' : size, 'name' : 'def', 'mode' : {'initial' : select_mode } }
        
        form = IngestionForm(default_values)
        return render_to_response('wizard/ingestion.html', {
                                                    'form': form, 
                                                    },context_instance=RequestContext(request))   
    else:
        return 'error.html'

@csrf_protect
def preprocessing_detail(request,pk):
    template = Template.objects.get(id=pk)
    if template:
        attributes = template.attribute_values
        for label in attributes:
            for key in label.attribute:
                if key == 'Extension Filter':
                    extension_options = label.attribute[key]
                    extension_enabled = extension_options['Enabled'] 
                    extension_values = extension_options['Values']
                elif key == 'Third Party Filter':
                    thirdparty_options = label.attribute[key]
                    thirdparty__enabled = extension_options['Enabled'] 
                    thirdparty__values = extension_options['Values']
                elif key == 'Personal Info Filter':
                    personalInfo_options = label.attribute[key]
                    personalInfo_enabled = extension_options['Enabled'] 
                    personalInfo_values = extension_options['Values']
                
        form = PreProcessingForm()
        return render_to_response('wizard/preProcessing.html', {
                                                    'form': form, 
                                                    },context_instance=RequestContext(request))   
    else:
        return 'error.html'
    
KPIS_CHOICES=(('Visitors per Domain','Visitors per Domain'),
              ('Visitors per Device','Visitors per Device'),
              ('Visitors per Protocol','Visitors per Protocol'))
GROUPING_FIELDS_CHOICES=(('1','Enabled'),('0','Disabled'))
ATTRIBUTE_FIELDS_CHOICES=(('visitorId','visitorId'),('protocol','protocol'),('fullUrl','fullUrl'),('status','status'),('urlDomain','urlDomain'))
CONSUMPTION_MODE_CHOICES=(('highAvailability','High Availability'),('dataExploitation','Data Exploitation'))
@csrf_protect
def webprofiling_detail(request,pk):
    template = Template.objects.get(id=pk)
    if template:
        attributes = template.attribute_values
        for label in attributes:
            for key in label.attribute:
                if key == 'ConsumptionMode':
                    choices = label.attribute[key]
                    if choices['highAvailability'] == 1: 
                        select_mode = 'highAvailability' 
                    else: select_mode = 'dataExploitation'
                elif key == 'Grouping Fields':
                    grp_fields = label.attribute[key]
                elif key == 'Attributes Fields':
                    atr_fields = label.attribute[key]
                elif key == 'Consumption Path':
                    path = label.attribute[key]
                
        #default_values = {'path' : path, 'size' : size, 'name' : 'def', 'mode' : {'initial' : select_mode } }
        
        form = WebProfilingForm()
        return render_to_response('wizard/webProfiling.html', {
                                                    'form': form, 
                                                    },context_instance=RequestContext(request))   
    else:
        return 'error.html'