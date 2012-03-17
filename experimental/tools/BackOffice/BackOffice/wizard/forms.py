from django.db import models
from django.forms import ModelForm
from BackOffice.wizard.models import Ingestion,Consumption

# Create your form from Ingestion Model
class IngestionForm(ModelForm):
    error_css_class = 'error'
    required_css_class = 'required'
    class Meta:
            model = Ingestion
    
# Create your form from Consumption Model
class ConsumptionForm(ModelForm):
    error_css_class = 'error'
    required_css_class = 'required'
    class Meta:
            model = Consumption

# Create FormWizard
from django.shortcuts import render_to_response
from django.contrib.formtools.wizard import FormWizard

class ConfigurationWizard(FormWizard):
    def done(self, request, form_list):
        for form in form_list:
            if form.is_valid(): # All validation rules pass
                # Process the data in form.cleaned_data
                # ...
                #form.cleaned_data['name']
                #form.cleaned_data['streaming']
                #form.cleaned_data['file']
                #form.cleaned_data['estimated_size']
                form.save('true')
        return render_to_response('forms/finish.html', {
                                                        'form_data': [form.cleaned_data for form in form_list],
                                                        })
        
# Note that this method will be called via POST, so it really ought to be a good Web citizen and redirect after processing the data. Here's another example
# do_something_with_the_form_data(form_list)
# return HttpResponseRedirect('/page-to-redirect-to-when-done/')