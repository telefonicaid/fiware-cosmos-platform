<%namespace name="shared" file="shared_components.mako" />

${shared.header("Cosmos", section="job_runs")}

## this id in the div below ("index") is stripped by Hue.JFrame
## and passed along as the "view" argument in its onLoad event

## the class 'jframe_padded' will give the contents of your window a standard padding
<div id="index" class="view">
  <table data-filters="HtmlTable" class="job-listing selectable sortable"
         cellpadding="0" cellspacing="0">
    <thead>
      <tr>
        <th>Date</th>
        <th>Job name</th>
        <th>Description</th>
        <th>Status</th>
        <th>Actions</th>
      </tr>
    </thead>
    <tbody>
      % for job_run in job_runs:
      <tr>
        <td class="job-start">${job_run.start_date | h}</td>
        <td class="job-name">${job_run.name | h}</td>
        <td class="job-description">${job_run.description | h}</td>
        <td class="job-status">${job_run.state() | h}</td>
        <td class="job-actions">
            % for i, link in enumerate(job_run.action_links()):
            % if i > 0:
            |
            % endif
            <a href="${ link['href'] }"
               class="job-action ${ link['class'] }"
               % if link['target'] is not None:
               target="${ link['target'] }"
               % endif
               >${ link['name'] }</a>
            % endfor
        </td>
      </tr>
      % endfor
    </tbody>
  </table>
</div>

${shared.footer()}
## vim:set syntax=mako:
