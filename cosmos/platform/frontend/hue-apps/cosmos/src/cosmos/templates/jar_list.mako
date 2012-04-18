<%namespace name="shared" file="shared_components.mako" />

${shared.header("Cosmos", section="jars")}

<div id="index" class="view jframe_padded">
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
      % for jar in jars:
      <tr>
        <td>${jar.name | h}</td>
        <td>${jar.description | h}</td>
        <td/>
      </tr>
      % endfor
    </tbody>
  </table>
</div>

${shared.footer()}
## vim:set syntax=mako:
