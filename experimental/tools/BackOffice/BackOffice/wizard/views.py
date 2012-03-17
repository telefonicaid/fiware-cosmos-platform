# Create your views here.
from BackOffice.wizard.forms import IngestionForm
from django.shortcuts import render_to_response
from django.http import HttpResponseRedirect
from django.views.decorators.csrf import csrf_protect
from django.template import RequestContext

@csrf_protect
def ingestion(request):
    c = {}
    if request.method == 'POST': # If the form has been submitted...
        form = IngestionForm(request.POST) # A form bound to the POST data
        if form.is_valid(): # All validation rules pass
            # Process the data in form.cleaned_data
            # ...
            form.cleaned_data['name']
            form.cleaned_data['streaming']
            form.cleaned_data['file']
            form.cleaned_data['estimated_size']
            commit = 'true';
            form.save(commit)
            return render_to_response('forms/finish.html')#HttpResponseRedirect('/forms/finish.html') # Redirect after POST
    else:
        form = IngestionForm() # An unbound form

    return render_to_response('wizard/ingestion.html', {
        'form': form, 
    },context_instance=RequestContext(request))