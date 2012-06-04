<%namespace name="shared" file="shared_components.mako" />
<%def name="render_error(err)">
  <div class="jframe-error">
    ${unicode(err) | n}
  </div>
</%def>

${shared.header("Run new job", section="define_job")}

<div id="run_job" class="jframe_padded view">
    <h4 class="jframe-hidden">Run new job</h4>

    <p>Please, review job settings.</p>

    <table>
	<thead>
	    <tr>
		<th>Property</th>
		<th>Value</th>
	    </tr>
	</thead>
	<tbody>
	    <tr>
		<th>Name</th>
		<td>${ job['name'] }</td>
	    </tr>
	    <tr>
		<th>Description</th>
		<td>${ job['description'] }</td>
	    </tr>
	    <tr>
		<th>JAR file</th>
		<td>${ job['jar_path'] }</td>
	    </tr>
	    <tr>
		<th>Dataset path</th>
		<td>${ job['dataset_path'] }</td>
	    </tr>
	</tbody>
    </table>

    <form action="${ url('confirm_job') }" method="POST" class="cos-run_job_form"
          enctype="multipart/form-data">
	<br/>
	<a class="cos-cancel" href="${ url('cancel_job') }">Cancel</a>
	<input class="submit" name="back" type="submit" value="Back" />
	<input class="submit" name="next" type="submit" value="Run job" />
    </form>
</div>

${shared.footer()}
## vim:set syntax=mako:
