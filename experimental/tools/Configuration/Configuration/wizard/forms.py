from django import forms
from django.forms.fields import ChoiceField
from django.forms.widgets import RadioSelect
from django.contrib.formtools.wizard import FormWizard
from django.shortcuts import render_to_response
from Configuration.wizard.models import Template, Label

# Create your form from Ingestion Model
MODE_CHOICES=(('Streaming','Streaming'),('File','File'))
class IngestionForm(forms.Form):
    error_css_class = 'error'
    required_css_class = 'required'
    name = forms.CharField(max_length=100)
    mode = ChoiceField(widget=RadioSelect, choices=MODE_CHOICES)
    path = forms.CharField(max_length=200)
    size = forms.IntegerField()
    def validate_form(self,form):
        if self.is_valid(): # All validation rules pass
            # Process the data in form.cleaned_data
            # ...
            #form.cleaned_data['name']
            #commit = 'true';
            #form.save(commit)
            lbl1 = Label()
            lbl1.header = "IngestionMode"
            if self['mode'].value() == "Streaming":
                options = "1,0"
            else:
                options = "0,1"   
                
            lbl1.options = options
            lbl2 = Label()
            lbl2.header = "IngestionAddress"
            lbl2.options = self['path'].value()
            lbl3 = Label()
            lbl3.header = "IngestionSize"
            lbl3.options = self['size'].value()
            
            tpl = Template()
            tpl.template = "IngestionTemplate"
            tpl.attribute_values = [lbl1,lbl2,lbl3]
            tpl.save()
        else:
            return render_to_response('forms/error1.html')        
        
MODE_CHOICES=(('Data Exploitation','Data Exploitation'),('High Availability','High Availability'))
class ConsumptionForm(forms.Form):
    error_css_class = 'error'
    required_css_class = 'required'
    mode = ChoiceField(widget=RadioSelect, choices=MODE_CHOICES)
    path = forms.CharField(max_length=200)
    def validate_form(self,form):
        if self.is_valid(): # All validation rules pass
            # Process the data in form.cleaned_data
            # ...
            #form.cleaned_data['name']
            #commit = 'true';
            #form.save(commit)
            lbl1 = Label()
            lbl1.header = "ConsumptionMode"
            if self['mode'].value() == "Data Exploitation":
                options = "1,0"
            else:
                options = "0,1"                   
            lbl1.options = options
            
            lbl2 = Label()
            lbl2.header = "ConsumptionPath"
            lbl2.options = self['path'].value()
            
            tpl = Template()
            tpl.template = "ConsumptionTemplate"
            tpl.attribute_values = [lbl1,lbl2]
            tpl.save()
        else:
            return render_to_response('forms/error2.html')


class ConfigurationWizard(FormWizard):
    def done(self, request, form_list):
        for form in form_list:
            form.validate_form(form)
        return render_to_response('forms/finish.html')
        