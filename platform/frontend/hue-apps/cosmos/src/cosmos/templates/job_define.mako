<%namespace name="shared" file="shared_components.mako" />
<%def name="render_error(err)">
  <div class="jframe-error">
    ${unicode(err) | n}
  </div>
</%def>

${shared.header("Run new job", section="define_job")}

<div id="run_job" class="jframe_padded view">
    <h4 class="jframe-hidden">Run new job</h4>

    <p>
    You are creating a new custom job by uploading a custom Hadoop mapreduce
    JAR.  Try the <a id="sample-jar-link" target="FileBrowser"
    href="/filebrowser/view/share/samples/jars/">available examples</a> (<a
    id="sample-jar-sources" href="/filebrowser/view/share/samples/src/"
    target="FileBrowser">sources</a>) or read about the <a
    id="jar-restrictions" href="/help/cosmos/" target="Help">custom job
    requirements</a>.
    </p>

    <form action="${ url('define_job') }" method="POST" class="cos-run_job_form"
          enctype="multipart/form-data">

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
        </dl>

        ${shared.wizard_navigation()}
    </form>
</div>


${shared.footer()}
## vim:set syntax=mako:
