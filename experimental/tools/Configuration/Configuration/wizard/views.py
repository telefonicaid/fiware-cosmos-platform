# Create your views here.
from Configuration.wizard.forms import IngestionForm
from Configuration.wizard.models import Template, Label
from django.shortcuts import render_to_response
from django.views.decorators.csrf import csrf_protect
from django.template import RequestContext

@csrf_protect
def ingestion(request):
    templates = Template.objects.filter(template = 'IngestionTemplate')
    return render_to_response('wizard/ingestion_list.html', {
                                                    'templates': templates, 
                                                    },context_instance=RequestContext(request))


@csrf_protect
def detail(request,pk):
    template = Template.objects.get(id=pk)
    if template:
        attributes = template.attribute_values
#        for label in attributes:
##            if label.attribute['IngestionMode']:
##                streaming = label.attribute['Streaming']
##                file = label.attribute['File']
#            if label.attribute['IngestionSize']:
#                size = label.attribute['IngestionSize']
#            elif label.attribute['IngestionAddress']:
#                path = label.attribute['IngestionAddress']
                
        default_values = {'path' : 'tofull', 'size' : 'tofull', 'name' : 'def', 'mode' : {'initial' : '0' } }
        
        form = IngestionForm(default_values)
        return render_to_response('wizard/ingestion.html', {
                                                    'form': form, 
                                                    },context_instance=RequestContext(request))   
    else:
        return 'error.html'
