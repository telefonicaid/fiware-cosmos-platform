# Create your views here.
from Configuration.wizard.forms import IngestionForm, ConsumptionForm
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
def consumption(request):
    if request.method == 'POST': # If the form has been submitted...
        form = ConsumptionForm(request.POST) # A form bound to the POST data
        if form.is_valid(): # All validation rules pass
            # Process the data in form.cleaned_data
            # ...
            #form.cleaned_data['name']
            #commit = 'true';
            #form.save(commit)
            lbl1 = Label()
            lbl1.header = "ConsumptionMode"
            if form['mode'].value() == "Data Exploitation":
                options = "1,0"
            else:
                options = "0,1"                   
            lbl1.options = options
            
            lbl2 = Label()
            lbl2.header = "ConsumptionPath"
            lbl2.options = form['path'].value()
            
            tpl = Template()
            tpl.template = "ConsumptionTemplate"
            tpl.attribute_values = [lbl1,lbl2]
            tpl.save()
            return render_to_response('forms/finish.html')#HttpResponseRedirect('/forms/finish.html') # Redirect after POST
    else:
        form = ConsumptionForm() # An unbound form

    return render_to_response('wizard/consumption.html', {
        'form': form, 
    },context_instance=RequestContext(request))