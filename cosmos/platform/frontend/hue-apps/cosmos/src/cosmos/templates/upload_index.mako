<%namespace name="shared" file="shared_components.mako" />
<%
    from django.core.urlresolvers import reverse
%>

${shared.header('Upload datasets and JARs', section='upload')}

<div id="index" class="view">
    <div class="cos-upload_choices">
	<p>
	    Upload your data and MapReduce tasks to the platform.
	</p>
	<ul>
	    <li><a href="${ url('upload_dataset') }" data-filters="ArtButton"
		   data-icon-styles="{'width': 16, 'height': 16, 'top':4,
		                      'left': 5}"
		   class="cos-upload_dataset">Upload dataset</a></li>
	    <li><a href="${ url('upload_jar') }" data-filters="ArtButton"
		   data-icon-styles="{'width': 16, 'height': 16, 'top':4,
		                      'left': 5}"
		   class="cos-upload_jar">Upload jar</a></li>
	</ul>
    </div>
</div>

${shared.footer()}
## vim:set syntax=mako:
