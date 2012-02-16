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
                
        form = IngestionForm(initial = {'path' : path, 'size' : size, 'name' : 'def', 'mode' :select_mode})
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
                    thirdparty__enabled = thirdparty_options['Enabled'] 
                    thirdparty__values = thirdparty_options['Values']
                elif key == 'Personal Info Filter':
                    personalInfo_options = label.attribute[key]
                    personalInfo_enabled = personalInfo_options['Enabled'] 
                    personalInfo_values = personalInfo_options['Values']
                
        form = PreProcessingForm(initial = {'extension' : extension_enabled, 'extension_values' :extension_values, 
                                            'thirdParty' : thirdparty__enabled, 'thirdParty_values' : thirdparty__values,
                                            'personalInfo' : personalInfo_enabled, 'personalInfo_values' :personalInfo_values})
        return render_to_response('wizard/preProcessing.html', {
                                                    'form': form, 
                                                    },context_instance=RequestContext(request))   
    else:
        return 'error.html'
    
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
                    attr_fields = label.attribute[key]
                elif key == 'Consumption Path':
                    path = label.attribute[key]
                
        form = WebProfilingForm({'mode' : select_mode, 'consumption_path' :path, 
                                'grouping_fields' : grp_fields, 
                                'attributes_fields' : attr_fields})
        
        return render_to_response('wizard/webProfiling.html', {
                                                    'form': form, 
                                                    },context_instance=RequestContext(request))   
    else:
        return 'error.html'