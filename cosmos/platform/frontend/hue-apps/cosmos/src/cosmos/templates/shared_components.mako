<%!
def is_selected(section, matcher):
  if section == matcher:
    return "selected"
  else:
    return ""
%>

<%!
from django.template.defaultfilters import urlencode, escape
%>
<%def name="toolbar_icon(id, name, url, extra_classes)">
<li><a href="${url}" class="nav_icon cos-${id}_nav ${extra_classes}" data-filters="ArtButton" data-icon-styles="{'width': 16, 'height': 16, 'top':4, 'left': 5}">${name}</a></li>
</%def>

<%def name="simple_header(title='Cosmos')">
  <!DOCTYPE html>
  <html>
    <head>
      <title>${title}</title>
    </head>
    <body class="hue-shared">
</%def>

<%def name="header(title='Cosmos', toolbar=True, section=False)">
  ${simple_header(title)}
  <div class="toolbar">
    <a href="${url('cosmos.views.index')}"><img src="/cosmos/static/art/cosmos-logo.png" class="cosmos_icon" alt="Cosmos"/></a>
    % if toolbar:
    <ul class="nav" data-filters="ArtButtonBar">
      ${toolbar_icon('job_runs', 'Job runs', url('cosmos.views.index'),
                     is_selected(section, 'job_runs'))}
      ${toolbar_icon('define_job', 'New job', url('define_job'),
                     is_selected(section, 'run_job'))}
      ${toolbar_icon('upload', 'Upload',
                     url('cosmos.views.upload_index'),
                     is_selected(section, 'upload'))}
      <li>
	<a class="jframe-refresh large" data-filters="ArtButton">Refresh</a>
      </li>
    </ul>
    % endif
  </div>
  <hr class="jframe-hidden" />
</%def>

<%def name="footer()">
    </body>
  </html>
</%def>

<%doc>
    Wizard components
</%doc>

<%def name="wizard_navigation()">
    <br/>
    <div class="cos-wizard_nav">
	<a class="cos-cancel" href="${ url('cancel_job') }">Cancel</a>
	% if wizard_nav.has_key('finish'):
	<input class="submit" name="next" type="submit"
	       value="${ wizard_nav['finish'] }" />
	% endif
	% if wizard_nav['next']:
	<input class="submit" name="next" type="submit" value="Next >" />
	% endif
	% if wizard_nav['back']:
	<input class="submit" name="back" type="submit" value="< Back" />
	% endif
    </div>
</%def>
## vim:set syntax=mako:
