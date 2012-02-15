# Create your views here.
from Configuration.wizard.forms import IngestionForm, PreProcessingForm
from Configuration.wizard.models import Template, Label
from django.shortcuts import render_to_response
from django.views.decorators.csrf import csrf_protect
from django.template import RequestContext

@csrf_protect
def ingestion(request):
    if request.method == 'POST': # If the form has been submitted...
        form = IngestionForm(request.POST) # A form bound to the POST data
        if form.is_valid(): # All validation rules pass
            # Process the data in form.cleaned_data
            # ...
            #form.cleaned_data['name']
            #commit = 'true';
            #form.save(commit)
            lbl1 = Label()
            lbl1.header = "IngestionMode"
            if form['mode'].value() == "Streaming":
                options = "1,0"
            else:
                options = "0,1"   
                
            lbl1.options = options
            lbl2 = Label()
            lbl2.header = "IngestionAddress"
            lbl2.options = form['path'].value()
            lbl3 = Label()
            lbl3.header = "IngestionSize"
            lbl3.options = form['size'].value()
            
            tpl = Template()
            tpl.template = "IngestionTemplate"
            tpl.attribute_values = [lbl1,lbl2,lbl3]
            tpl.save()
            return render_to_response('forms/finish.html')#HttpResponseRedirect('/forms/finish.html') # Redirect after POST
    else:
        form = IngestionForm() # An unbound form

    return render_to_response('wizard/ingestion.html', {
        'form': form, 
    },context_instance=RequestContext(request))
    
@csrf_protect
def preProcessing(request):
    if request.method == 'POST': # If the form has been submitted...
        form = PreProcessingForm(request.POST) # A form bound to the POST data
        if form.is_valid(): # All validation rules pass
            # Process the data in form.cleaned_data
            # ...
            #form.cleaned_data['name']
            #commit = 'true';
            #form.save(commit)
            lbl1 = Label()
            lbl1.header = "Extension Filter"
            lbl1.options = form['extension'].value()
            
            lbl2 = Label()
            lbl2.header = "Third Party Filter"
            lbl2.options = form['thirdParty'].value()
            
            lbl3 = Label()
            lbl3.header = "Personal Info Filter"
            lbl3.options = form['personalInfo'].value()
            
            tpl = Template()
            tpl.template = "PreprocessingTemplate"
            tpl.attribute_values = [lbl1,lbl2,lbl3]
            tpl.save()
            return render_to_response('forms/finish.html')#HttpResponseRedirect('/forms/finish.html') # Redirect after POST
        else:
            form = PreProcessingForm() # An unbound form

    return render_to_response('wizard/preProcessing.html', {
        'form': form, 
    },context_instance=RequestContext(request))