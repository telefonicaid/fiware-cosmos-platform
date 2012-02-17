# Create your views here.
from Configuration.wizard.forms import IngestionForm, PreProcessingForm, WebProfilingForm
from django.shortcuts import render_to_response
from django.views.decorators.csrf import csrf_protect
from django.template import RequestContext
from django.http import HttpResponseRedirect


@csrf_protect
def ingestion(request):
    if request.method == 'GET':
        form = IngestionForm()
        return render_to_response('wizard/ingestion.html', {
                                                    'form': form
                                                    },context_instance=RequestContext(request))
        
    elif request.method == 'POST': # If the form has been submitted...
        form = IngestionForm(request.POST) # A form bound to the POST data
        if form.is_valid():
            template_form = form.create_template_model()
            template_form.save()
            return HttpResponseRedirect('../')
        else:
            form = IngestionForm(request.POST)   
    
@csrf_protect
def preprocessing(request):
    if request.method == 'GET':    
        form = PreProcessingForm()
        return render_to_response('wizard/preProcessing.html', {
                                                    'form': form
                                                    },context_instance=RequestContext(request))
    elif request.method == 'POST': # If the form has been submitted...
        form = PreProcessingForm(request.POST) # A form bound to the POST data
        if form.is_valid():
            template_form = form.create_template_model()
            template_form.save()
            return PreProcessingForm('../')
        else:
            form = IngestionForm(request.POST)   
    
@csrf_protect
def webprofiling(request):
    if request.method == 'GET':
        form = WebProfilingForm()
        return render_to_response('wizard/webProfiling.html', {
                                                    'form': form
                                                    },context_instance=RequestContext(request))
    elif request.method == 'POST': # If the form has been submitted...
        form = WebProfilingForm(request.POST) # A form bound to the POST data
        if form.is_valid():
            template_form = form.create_template_model()
            template_form.save()
            return HttpResponseRedirect('../')
        else:
            form = WebProfilingForm(request.POST)   
    
