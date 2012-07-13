<%namespace name="shared" file="shared_components.mako" />
<%def name="render_error(err)">
  <div class="jframe-error">
    ${unicode(err) | n}
  </div>
</%def>

${shared.header("Run new job", section="define_job")}

<div id="run_job" class="jframe_padded view">
    <h4 class="jframe-hidden">Run new job</h4>

    <p>Please, configure your job.  You can use variable expansion, for
    instance ${"${"} user.home }. Check the supported variables on the <a
    href="/help/cosmos" target="Help">documentation page</a>.</p>

    <form action="${ url('configure_job') }" method="POST" class="cos-run_job_form"
          enctype="multipart/form-data">

        % for err in form.non_field_errors():
        ${render_error(err)}
        % endfor

        ${ form.as_p() | n }

        ${shared.wizard_navigation()}
    </form>
</div>

${shared.footer()}
## vim:set syntax=mako:
