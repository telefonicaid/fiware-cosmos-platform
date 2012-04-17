<%namespace name="shared" file="shared_components.mako" />

${shared.header("Cosmos", section="job_runs")}

## this id in the div below ("index") is stripped by Hue.JFrame
## and passed along as the "view" argument in its onLoad event

## the class 'jframe_padded' will give the contents of your window a standard padding
<div id="index" class="view">
  <table data-filters="HtmlTable" class="selectable sortable" cellpadding="0"
         cellspacing="0">
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
        <td>${job_run.start_date | h}</td>
        <td>${job_run.name | h}</td>
        <td>${job_run.description | h}</td>
        <td>${job_run.status | h}</td>
	<td>
            <!-- TODO: show links to progress in job browser, results in
                    cosmos_dataviewer and error in here -->
            <a target="CosmosDataviewer"
	       href="/cosmos_dataviewer/run/1">Results</a>
        </td>
      </tr>
      % endfor
    </tbody>
  </table>
</div>

${shared.footer()}
## vim:set syntax=mako:
