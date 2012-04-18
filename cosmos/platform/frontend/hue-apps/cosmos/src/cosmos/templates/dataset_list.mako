<%namespace name="shared" file="shared_components.mako" />

${shared.header("Cosmos", section="datasets")}

## this id in the div below ("index") is stripped by Hue.JFrame
## and passed along as the "view" argument in its onLoad event

## the class 'jframe_padded' will give the contents of your window a standard padding
<div id="index" class="view">
  <table data-filters="HtmlTable" class="selectable sortable" cellpadding="0"
         cellspacing="0">
    <thead>
      <tr>
        <th>Name</th>
	<th>Description</th>
	<th>Actions</th>
      </tr>
    </thead>
    <tbody>
      % for dataset in datasets:
      <tr>
        <td>${dataset.name | h}</td>
        <td>${dataset.description | h}</td>
        <td/>
      </tr>
      % endfor
    </tbody>
  </table>
</div>

${shared.footer()}
## vim:set syntax=mako:
