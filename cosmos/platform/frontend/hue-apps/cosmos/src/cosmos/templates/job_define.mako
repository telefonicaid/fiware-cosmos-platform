<%namespace name="shared" file="shared_components.mako" />
<%def name="render_error(err)">
  <div class="jframe-error">
    ${unicode(err) | n}
  </div>
</%def>

${shared.header("Run new job", section="define_job")}

<div id="run_job" class="jframe_padded view">
    <form action="${ url('define_job') }" method="POST" class="cos-run_job_form"
          enctype="multipart/form-data">
	<h4 class="jframe-hidden">Run new job</h4>

	<p>
	You are creating a new custom job by uploading a custom Hadoop mapreduce
	JAR.  Try the <a id="sample-jar-link" target="_blank"
	href="/cosmos/static/samples/wordcount.jar">wordcount example</a> (<a 
        id="sample-jar-sources" href="/cosmos/static/samples/wordcount.tgz"
	target="_blank">sources</a>) or read about the
	<a id="jar-restrictions" href="/help/cosmos/"
	target="Help">custom job requirements</a>.
	</p>

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
	    <dd class="file_finder">
		${ unicode(form['jar_path']) | n }
		<a class="hue-choose_file" data-filters="ArtButton"
		   data-chooseFor="jar_path">...</a>
		${ unicode(form['jar_path'].errors) | n }
	    </dd>

            <dt><label for="dataset_path">Dataset path</label></dt>
	    <dd class="file_finder">
		${ unicode(form['dataset_path']) | n }
		<a class="hue-choose_file" data-filters="ArtButton"
		   data-chooseFor="dataset_path">...</a>
		${ unicode(form['dataset_path'].errors) | n }
	    </dd>
        </dl>

	<br/>
	<input class="submit" name="cancel" type="submit" value="Cancel" />
	<input class="submit" name="next"   type="submit" value="Next >" />
    </form>
</div>


${shared.footer()}
## vim:set syntax=mako:
