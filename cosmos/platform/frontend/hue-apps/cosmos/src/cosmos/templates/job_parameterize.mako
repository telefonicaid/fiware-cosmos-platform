<%namespace name="shared" file="shared_components.mako" />
<%def name="render_error(err)">
  <div class="jframe-error">
    ${unicode(err) | n}
  </div>
</%def>

${shared.header("Run new job", section="define_job")}

<div id="run_job" class="jframe_padded view">
    <form action="${ url('configure_job') }" method="POST" class="cos-run_job_form"
          enctype="multipart/form-data">
	<h4 class="jframe-hidden">Run new job</h4>

	<p>This job requires further parametrization.</p>

        % for err in form.non_field_errors():
        ${render_error(err)}
        % endfor

	${ form.as_p() | n }

	<br/>
	<a class="cos-cancel" href="${ url('cancel_job') }">Cancel</a>
	<input class="submit" name="back" type="submit" value="Back" />
	<input class="submit" name="next"   type="submit" value="Next >" />
    </form>
</div>

${shared.footer()}
## vim:set syntax=mako:
