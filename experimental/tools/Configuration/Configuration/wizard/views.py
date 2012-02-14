# Create your views here.
from Configuration.wizard.forms import IngestionForm, AttributeForm, LabelForm
from Configuration.wizard.models import Attribute, Label
from django.shortcuts import render_to_response
from django.views.decorators.csrf import csrf_protect
from django.template import RequestContext
from django.forms.models import modelformset_factory


@csrf_protect
def attribute(request):
    AttributeFormSet = modelformset_factory(Attribute)
    LabelFormSet = modelformset_factory(Label)
    if request.method == 'POST':
        attribute_formset = AttributeFormSet(request.POST, request.FILES, prefix='attributes')
        label_formset = LabelFormSet(request.POST, request.FILES, prefix='labels')
        if attribute_formset.is_valid() and label_formset.is_valid():
            # do something with the cleaned_data on the formsets.
            attribute_formset.save('true')
            label_formset.save('true')
            return render_to_response('forms/finish.html')
    else:
        attribute_formset = AttributeFormSet(prefix='attributes')
        label_formset = LabelFormSet(prefix='labels')
    return render_to_response('wizard/ingestion2.html', {
        'attribute_formset': attribute_formset,
        'label_formset': label_formset,
    },context_instance=RequestContext(request))
    
@csrf_protect
def ingestion(request):
    if request.method == 'POST': # If the form has been submitted...
        form = IngestionForm(request.POST) # A form bound to the POST data
        if form.is_valid(): # All validation rules pass
            # Process the data in form.cleaned_data
            # ...
            form.cleaned_data['identifier']
            form.cleaned_data['type']
            commit = 'true';
            form.save(commit)
            return render_to_response('forms/finish.html')#HttpResponseRedirect('/forms/finish.html') # Redirect after POST
    else:
        form = IngestionForm() # An unbound form

    return render_to_response('wizard/ingestion.html', {
        'form': form, 
    },context_instance=RequestContext(request))