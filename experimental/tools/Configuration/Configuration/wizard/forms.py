from django import forms
from django.forms.fields import ChoiceField
from django.forms.widgets import RadioSelect,CheckboxSelectMultiple, Select, DateInput
from django.contrib.formtools.wizard import FormWizard
from django.shortcuts import render_to_response
from Configuration.wizard.models import Template, Label, Configuration
from djangotoolbox.fields import ListField
from django.http import HttpResponseRedirect
import datetime


class ConfigurationForm(forms.Form):
    name = forms.CharField( widget=forms.TextInput(attrs={'max_length':100, 'readonly':'readonly'}))
    
    def __init__(self, templates,*args, **kwargs):
        super(ConfigurationForm, self).__init__(*args, **kwargs)
        if templates:
            self.fields['templates'] = forms.ModelMultipleChoiceField(queryset=Template.objects.filter(id__in=templates))
        else:
            self.fields['templates'] = forms.ModelMultipleChoiceField(queryset=Template.objects.all())
        
    # Creates Template model from PreProcessing Form data
    # return Model
    def create_config_model(self):
        cfg = Configuration( name = self.cleaned_data['name'])
        return cfg.id
    
    # Saves Ingestion Form, really saves Template Model
    # return pk
    def save_form(self,form):
        if self.is_valid(): # All validation rules pass
            config = self.create_template_model() 
            config.save()
            return config.id
        else:
            return render_to_response('forms/error.html')    
    
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
    initial_date = forms.DateField(initial=datetime.date.today, input_formats='%d/%m/%Y')
    end_date = forms.DateField(input_formats='%d/%m/%Y')
    frequency = forms.CharField(initial='1',required=False)
    output = forms.CharField(max_length=200)
    file_name = forms.CharField(max_length=200)
    
        
    # Creates Template Model from Ingestion Form data
    def create_template_model(self):
        # Create Template
        dict = {}
        for choice in INGESTION_MODE_CHOICES:
            if self['mode'].value() == "Streaming":
                dict[choice[0]] = 1
            else:
                dict[choice[0]] = 0
        lbl1 = Label()
        lbl1.attribute['IngestionMode'] = dict  
        lbl2 = Label()
        lbl2.attribute["LandingArea"] = self['path'].value()
        lbl3 = Label()
        lbl3.attribute["IngestionSize"] = self['size'].value()
        lbl4 = Label()
        lbl4.attribute["ConfigurationName"] = self['name'].value()
        lbl5 = Label()
        lbl5.attribute["InitialDate"] = self['initial_date'].value()
        lbl6 = Label()
        lbl6.attribute["EndDate"] = self['end_date'].value()
        lbl7 = Label()
        lbl7.attribute["UploadFrequency"] = self['frequency'].value()
        lbl8 = Label()
        lbl8.attribute["OutputPath"] = self['output'].value()
        lbl9 = Label()
        lbl9.attribute["FileNamePattern"] = self['file_name'].value()
        tpl = Template( template = "IngestionTemplate", attribute_values = [lbl1,lbl2,lbl3,lbl4,lbl5,lbl6,lbl7,lbl8,lbl9] )
        return tpl

    # Saves Ingestion Form, really saves Template Model
    # return pk
    def save_form(self,form):
        if self.is_valid(): # All validation rules pass
            template = self.create_template_model() 
            template.save()
            return template.id
        else:
            return render_to_response('forms/error.html')

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
EXTENSION_VALUES=(('jpg','jpg'),('mp3','mp3'),('png','png'))
THIRD_PARTY_VALUES=(('admob_com','admob.com'),('admob_com','admob.com'))
PERSONAL_INFO_VALUES=(('pornhub_com','pornhub.com'),('porntube.com','porntube.com'))

class PreProcessingForm(forms.Form):
    error_css_class = 'error'
    required_css_class = 'required'
    extension = ChoiceField(initial='0',widget=RadioSelect, choices=FILTERS_CHOICES)
    extension_values = forms.MultipleChoiceField(choices=EXTENSION_VALUES)
    thirdParty = ChoiceField(initial='0',widget=RadioSelect, choices=FILTERS_CHOICES)
    thirdParty_values = forms.MultipleChoiceField(choices=THIRD_PARTY_VALUES)
    personalInfo = ChoiceField(initial='0',widget=RadioSelect, choices=FILTERS_CHOICES)
    personalInfo_values = forms.MultipleChoiceField(choices=PERSONAL_INFO_VALUES)
    
    # Creates Template model from PreProcessing Form data
    # return Model
    def create_template_model(self):
        lbl1 = Label()
        
        lbl1.attribute["Extension Filter"] = {"Enabled": self["extension"].value(), 
                                                     "Values" : self['extension_values'].value()  }
        
        lbl2 = Label()
        lbl2.attribute["Third Party Filter"] = {"Enabled": self["thirdParty"].value(),
                                                              "Values" : self['thirdParty_values'].value() }
    
        lbl3 = Label()
        lbl3.attribute["Personal Info Filter"] = {"Enabled": self["personalInfo_values"].value(),
                                                                "Values" :  self['personalInfo_values'].value() } 
        
        tpl = Template(template = "PreProcessingTemplate", attribute_values = [lbl1,lbl2,lbl3])
        return tpl
    
    # Saves PreProcessing Form, really saves Template Model
    # return pk
    def save_form(self,form):
        if self.is_valid(): # All validation rules pass
            template = self.create_template_model() 
            template.save()
            return template.id
        else:
            return render_to_response('forms/error.html')
        
# Form used to configure Ingestion data
# It also has a validate_form that is called to 
# wrapper IngestionForm values into IngestionTemplate Model
# that's the reason why Form is no saved, it's used like a wrapper

# WebProfiling Mongo Example
# {"_id":ObjectId("xxxxxxx"),
# "template":"WebProfilingTemplate",
# "attribute_values":[{"header":"Name", "options":"Visitors per Domain"},
# {"header":"Grouping Fields","options":[{"visitorId":0},{"protocol":1},{"urlDomain":1},.......{"status":0}]},
# {"header":"Attribute Fields","options":[{"visitorId":1},{"protocol":0},{"fullUrl":0},.......{"status":0}]},
# {"header":"Consumption","options":[{"highAvailability":0},{"dataExploitation":1}]}]

KPIS_CHOICES=(('Visitors per Domain','Visitors per Domain'),
              ('Visitors per Device','Visitors per Device'),
              ('Visitors per Protocol','Visitors per Protocol'))
GROUPING_FIELDS_CHOICES=(('1','Enabled'),('0','Disabled'))
ATTRIBUTE_FIELDS_CHOICES=(('visitorId','visitorId'),('protocol','protocol'),('fullUrl','fullUrl'),('urlDomain','urlDomain'),('urlPath','urlPath'),('urlQuery','urlQuery'),('date','date'),('userAgent','userAgent'),('browser','browser'),('device','device'),('operSys','operSys'),('method','method'),('status','status'))
CONSUMPTION_MODE_CHOICES=(('highAvailability','High Availability'),('dataExploitation','Data Exploitation'))
class WebProfilingForm(forms.Form):
    error_css_class = 'error'
    required_css_class = 'required'
    mode = ChoiceField(widget=RadioSelect, choices=CONSUMPTION_MODE_CHOICES)
    grouping_fields = forms.ChoiceField( required=False, 
                                         widget=Select,
                                         choices=ATTRIBUTE_FIELDS_CHOICES)
    attributes_fields = forms.MultipleChoiceField( required=False, 
                                                   widget=CheckboxSelectMultiple,
                                                   choices=ATTRIBUTE_FIELDS_CHOICES)
    consumption_path = forms.CharField(max_length=200)
    job_name = forms.CharField(max_length=200)
    
    # Creates Template model from WebProfiling Form data
    # return Model
    def create_template_model(self):
            dict = {}
            for choice in CONSUMPTION_MODE_CHOICES:
                if self['mode'].value() == "highAvailability":
                    dict[choice[0]] = 1
                else:
                    dict[choice[0]] = 0
                    
            lbl1 = Label()
            lbl1.attribute['ConsumptionMode'] = dict 
            
        #    dict1 = {}
        #    dict1[self.cleaned_data['grouping_fields']] = 1
        #    lbl2 = Label()
        #    lbl2.attribute["Grouping Fields"] = dict1 
            
            dict1 = {}
            for option in self['grouping_fields'].value():
                dict1[option] = 1
            lbl2 = Label()
            lbl2.attribute["Grouping Fields"] = dict1
            
            dict2 = {}
            for option in self['attributes_fields'].value():
                dict2[option] = 1
            lbl3 = Label()
            lbl3.attribute["Attributes Fields"] = dict2
                
            lbl4 = Label()
            lbl4.attribute["Consumption Path"] = self["consumption_path"].value()
            
            lbl5 = Label()
            lbl5.attribute["Job Name"] = self["job_name"].value()
            
            tpl = Template( template = "WebProfilingTemplate", attribute_values = [lbl1,lbl2,lbl3,lbl4,lbl5])
            return tpl
        
    # Saves WebProfiling Form, really saves Template Model
    # return pk
    def save_form(self,form):
        if self.is_valid(): # All validation rules pass
            template = self.create_template_model() 
            template.save()
            return template.id
        else:
            return render_to_response('forms/error.html')

class WizardForm(FormWizard): 
    def process_step(self, request, form, step):
        stp = step
        if stp >= 3:
            return ['forms/wizard_%s.html' % step, 'wizard/preProcessing.html']
        
    def get_template(self,step):
        stp = step
        if stp == 0:
            return ['forms/wizard_%s.html' % step, 'wizard/ingestion.html']
        elif stp == 1:
            return ['forms/wizard_%s.html' % step, 'wizard/preProcessing.html']
        elif stp == 2:
            return ['forms/wizard_%s.html' % step, 'wizard/webProfiling.html']
        else:          
            return ['forms/wizard_%s.html' % step, 'wizard/webProfiling.html']  
        
    def done(self, request, form_list):
        config = Configuration()
        listField = []
        for form in form_list:
            listField.append(form.save_form(form))
        config.templates = listField
        config.name = form_list[0].cleaned_data['name']
        config.save()
        return HttpResponseRedirect('../')


RegisterWizard = WizardForm([IngestionForm, PreProcessingForm, WebProfilingForm]) 

def wizard(request): 
    if request.POST and 'wizard_continue' in request.POST: 
        RegisterWizard.form_list.append(WebProfilingForm)
    response = RegisterWizard(request) 
    
    hashes = [RegisterWizard.security_hash(request, RegisterWizard.get_form(i, request.POST)) for i in range(RegisterWizard.step)]  
    
    response['WizardHashes'] = hashes  
      
    return response