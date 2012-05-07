<%namespace name="shared" file="shared_components.mako" />
<%def name="render_error(err)">
  <div class="jframe-error">
    ${unicode(err) | n}
  </div>
</%def>

${shared.simple_header('Upload new dataset')}

<div class="prompt_popup">
    <form action="/cosmos/datasets/upload" method="POST" 
          enctype="multipart/form-data">
	<h4 class="jframe-hidden">Upload new dataset</h4>

        % for err in form.non_field_errors():
        ${render_error(err)}
        % endfor

        <p>
            ${ unicode(form['name'].errors) | n }
            <label for="name">Name</label>
            ${ unicode(form['name']) | n }
        </p>

        <p>
            ${ unicode(form['description'].errors) | n }
            <label for="description">Description</label>
            ${ unicode(form['description']) | n }
        </p>

        <p>
            ${ unicode(form['hdfs_file'].errors) | n }
            <label for="hdfs_file">File</label>
            ${ unicode(form['hdfs_file']) | n }
        </p>

	<input class="jframe-hidden" type="submit" value="Submit" />
    </form>

    <hr class="jframe-hidden" />

    <p class="jframe-hidden">No Flash plugin was detected. In order to improve
    dataset upload, please visit <a href="http://get.adobe.com/flashplayer/"
    target="_blank">Adobe site</a> to get the software for your system.</p>
</div>

${shared.footer()}
## vim:set syntax=mako:
