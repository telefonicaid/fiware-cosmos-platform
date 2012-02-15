from django import forms
from django.forms.fields import ChoiceField, MultipleChoiceField
from django.forms.widgets import RadioSelect,CheckboxSelectMultiple
from django.contrib.formtools.wizard import FormWizard
from django.shortcuts import render_to_response
from Configuration.wizard.models import Template, Label

# Form used to configure Ingestion data
# It also has a validate_form that is called to 
# wrapper IngestionForm values into IngestionTemplate Model
# that's the reason why Form is no saved, it's used like a wrapper
INGESTION_MODE_CHOICES=(('Streaming','Streaming'),('File','File'))
class IngestionForm(forms.Form):
    error_css_class = 'error'
    required_css_class = 'required'
    name = forms.CharField(max_length=100)
    mode = ChoiceField(widget=RadioSelect, choices=INGESTION_MODE_CHOICES)
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
            return render_to_response('forms/ingestionForm.html')        

# Form used to configure Filters data
# It also has a validate_form that is called to 
# wrapper PreProcessingForm values into PreProcessingTemplate Model
# that's the reason why Form is no saved, it's used like a wrapper
# {"_id":ObjectId("xxxxxxx"),
# "template":"PreprocessingTemplate","
# attribute_values":[{"header":"Extension Filter","options":[{"Enabled":1,"Values":["jpg","mp3","png"]}]},
# {"header":"Third Party Filter","options":[{"Enabled":1,"Values":["admob.com","adserve.es"]}]},
#  {"header":"Personal Info Filter","options":[{"Enabled":1,"Values":["pornhub.com","porntube.com"]}]}]}
FILTERS_CHOICES=(('1','Enabled'),('0','Disabled'))
EXTENSION_CHOICES=(('Enabled','Enabled'),('Disabled','Disabled'))
THIRD_PARTY_CHOICES=(('Enabled','Enabled'),('Disabled','Disabled'))
PERSONAL_INFO_CHOICES=(('Enabled','Enabled'),('Disabled','Disabled'))
class PreProcessingForm(forms.Form):
    error_css_class = 'error'
    required_css_class = 'required'
    extension = ChoiceField(widget=RadioSelect, choices=FILTERS_CHOICES)
    thirdParty = ChoiceField(widget=RadioSelect, choices=FILTERS_CHOICES)
    personalInfo = ChoiceField(widget=RadioSelect, choices=FILTERS_CHOICES)
    def validate_form(self,form):
        if self.is_valid(): # All validation rules pass
            # Process the data in form.cleaned_data
            # ...
            #form.cleaned_data['name']
            #commit = 'true';
            #form.save(commit)
            lbl1 = Label()
            lbl1.header = "Extension Filter"
            lbl1.options = self['extension'].value()
            
            lbl2 = Label()
            lbl2.header = "Third Party Filter"
            lbl2.options = self['thirdParty'].value()
            
            lbl3 = Label()
            lbl3.header = "Personal Info Filter"
            lbl3.options = self['personalInfo'].value()
            
            tpl = Template()
            tpl.template = "PreprocessingTemplate"
            tpl.attribute_values = [lbl1,lbl2,lbl3]
            tpl.save()
        else:
            return render_to_response('forms/preProcessingForm.html') 

# Form used to configure Ingestion data
# It also has a validate_form that is called to 
# wrapper IngestionForm values into IngestionTemplate Model
# that's the reason why Form is no saved, it's used like a wrapper
KPIS_CHOICES=(('1','Enabled'),('0','Disabled'))
GROUPING_FIELDS_CHOICES=(('1','Enabled'),('0','Disabled'))
ATTRIBUTE_FIELDS_CHOICES=(('visitorId','visitorId'),('protocol','protocol'),('fullUrl','fullUrl'),('status','status'),('urlDomain','urlDomain'))
CONSUMPTION_MODE_CHOICES=(('highAvailability','Enabled'),('dataExploitation','Disabled'))
class WebProfilingForm(forms.Form):
    error_css_class = 'error'
    required_css_class = 'required'
    mode = ChoiceField(widget=RadioSelect, choices=CONSUMPTION_MODE_CHOICES)
    path = forms.CharField(max_length=200)
    attributes_fields= forms.MultipleChoiceField(   required=False, 
                                                    widget=CheckboxSelectMultiple,
                                                    choices=ATTRIBUTE_FIELDS_CHOICES)
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
            tpl.template = "WebProfilingTemplate"
            tpl.attribute_values = [lbl1,lbl2]
            tpl.save()
        else:
            return render_to_response('forms/error2.html')


class ConfigurationWizard(FormWizard):        
    def done(self, request, form_list):
        for form in form_list:
            form.validate_form(form)
        return render_to_response('forms/finish.html')
        