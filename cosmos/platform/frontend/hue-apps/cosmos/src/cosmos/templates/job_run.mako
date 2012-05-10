<%namespace name="shared" file="shared_components.mako" />
<%def name="render_error(err)">
  <div class="jframe-error">
    ${unicode(err) | n}
  </div>
</%def>

${shared.simple_header("Run new job")}

<div class="prompt_popup">
    <form action="${ url('run_job') }" method="POST" class="cos-run_job_form"
          enctype="multipart/form-data">
       <h4 class="jframe-hidden">Run new job</h4>

        % for err in form.non_field_errors():
        ${render_error(err)}
        % endfor

        <dl>
	    <dt><label for="name">Name</label></dt>
	    <dd>
		${ unicode(form['name']) | n }
		${ unicode(form['name'].errors) | n }
	    </dd>

	    <dt><label for="description">Description</label></dt>
	    <dd>
		${ unicode(form['description']) | n }
		${ unicode(form['description'].errors) | n }
	    </dd>

            <dt><label for="jar_path">JAR file</label></dt>
	    <dd>
		${ unicode(form['jar_path']) | n }
		${ unicode(form['jar_path'].errors) | n }
	    </dd>

            <dt><label for="dataset_path">Dataset path</label></dt>
	    <dd>
		${ unicode(form['dataset_path']) | n }
		${ unicode(form['dataset_path'].errors) | n }
	    </dd>
        </dl>

	<input class="jframe-hidden" type="submit" value="Submit" />
    </form>
</div>


${shared.footer()}
## vim:set syntax=mako:
