<%!
#declare imports here, for example:
#import datetime
%>

<%!
import datetime
from django.template.defaultfilters import urlencode, escape
%>
<%def name="header(title='cosmos', toolbar=True)">
  <!DOCTYPE html>
  <html>
    <head>
      <title>${title}</title>
    </head>
    <body>
      % if toolbar:
      <div class="toolbar">
        <a href="${url('cosmos.views.index')}"><img src="/cosmos/static/art/cosmos.png" class="cosmos_icon"/></a>
      </div>
      % endif
</%def>

<%def name="footer()">
    </body>
  </html>
</%def>