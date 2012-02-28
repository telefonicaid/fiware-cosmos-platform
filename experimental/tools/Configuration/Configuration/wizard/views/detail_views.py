# Create your views here.
from Configuration.wizard.forms import IngestionForm, PreProcessingForm, WebProfilingForm, ConfigurationForm
from Configuration.wizard.models import Template, Configuration
from django.shortcuts import render_to_response
from django.views.decorators.csrf import csrf_protect
from django.template import RequestContext
from django.http import HttpResponseRedirect
    
@csrf_protect
def configuration_detail(request,pk):
    config = Configuration.objects.get(id=pk)
    queryset = Template.objects.filter(id__in=config.templates)
    if request.method == 'GET':
        form = ConfigurationForm(config.templates,initial={'name' : config.name})
#            initial={'name' : config.name}
        return render_to_response('wizard/configuration.html', {
                                                    'form': form, 'config' : config
                                                    },context_instance=RequestContext(request))   
    elif request.method == 'POST': # If the form has been submitted...
        form = ConfigurationForm(request.POST) # A form bound to the POST data
        if form.is_valid():
#            config_form = form.create_template_model()
#            template.attribute_values = template_form.attribute_values
#            template.save()
            return HttpResponseRedirect('../')
        else:
            form = IngestionForm(request.POST)
            
@csrf_protect
def ingestion_detail(request,pk):
    template = Template.objects.get(id=pk)
    if request.method == 'GET':
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
                    elif key == 'ConfigurationName':
                        name = label.attribute[key]
                    
            form = IngestionForm(initial = {'path' : path, 'size' : size, 'name' : name, 'mode' : select_mode})
            
            return render_to_response('wizard/ingestion.html', {
                                                        'form': form
                                                        },context_instance=RequestContext(request))   
            
    elif request.method == 'POST': # If the form has been submitted...
        form = IngestionForm(request.POST) # A form bound to the POST data
        if form.is_valid():
            template_form = form.create_template_model()
            template.attribute_values = template_form.attribute_values
            template.save()
            return HttpResponseRedirect('../')
        else:
            form = IngestionForm(request.POST)

@csrf_protect
def preprocessing_detail(request,pk):
    template = Template.objects.get(id=pk)
    if request.method == 'GET':
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
    elif request.method == 'POST': # If the form has been submitted...
        form = PreProcessingForm(request.POST) # A form bound to the POST data
        if form.is_valid():
            template_form = form.create_template_model()
            template.attribute_values = template_form.attribute_values
            template.save()
            return HttpResponseRedirect('../')
        else:
            form = PreProcessingForm(request.POST)
    
@csrf_protect
def webprofiling_detail(request,pk):
    template = Template.objects.get(id=pk)
    if request.method == 'GET':
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
                    elif key == 'Job Name':
                        job_name = label.attribute[key]
                    
            form = WebProfilingForm({'mode' : select_mode,
                                    'consumption_path' :path,
                                    'job_name' :job_name, 
                                    'grouping_fields' : grp_fields, 
                                    'attributes_fields' : attr_fields})
            
            return render_to_response('wizard/webProfiling.html', {
                                                        'form': form, 
                                                        },context_instance=RequestContext(request))   
    elif request.method == 'POST': # If the form has been submitted...
        form = WebProfilingForm(request.POST) # A form bound to the POST data
        if form.is_valid():
            template_form = form.create_template_model()
            template.attribute_values = template_form.attribute_values
            template.save()
            return HttpResponseRedirect('../')
        else:
            form = WebProfilingForm(request.POST)