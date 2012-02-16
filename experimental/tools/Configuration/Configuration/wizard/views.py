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
