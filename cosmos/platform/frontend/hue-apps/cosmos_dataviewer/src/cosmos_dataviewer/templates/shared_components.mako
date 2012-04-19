<%!
#declare imports here, for example:
#import datetime
%>

<%!
import datetime
from django.template.defaultfilters import urlencode, escape
%>
<%def name="header(title='cosmos_dataviewer', toolbar=True)">
  <!DOCTYPE html>
  <html>
    <head>
      <title>${title}</title>
    </head>
    <body>
      % if toolbar:
      <div class="toolbar">
        <a href="${url('cosmos_dataviewer.views.index')}"><img src="/cosmos_dataviewer/static/art/cosmos_dataviewer.png" class="cosmos_dataviewer_icon"/></a>
      </div>
      % endif
</%def>

<%def name="footer()">
    </body>
  </html>
</%def>