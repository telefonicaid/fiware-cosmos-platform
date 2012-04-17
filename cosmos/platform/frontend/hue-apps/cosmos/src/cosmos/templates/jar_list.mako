<%namespace name="shared" file="shared_components.mako" />

${shared.header("Cosmos", section="jars")}

<div id="index" class="view jframe_padded">
  <h2>My JARs</h2>
  <table data-filters="HtmlTable" class="selectable sortable" cellpadding="0"
         cellspacing="0">
    <thead>
      <tr>
        <th>Name</th>
	<th>Actions</th>
      </tr>
    </thead>
    <tbody>
      <tr>
        <td>wordcount.jar</td>
	<td><a href="#">Remove</a></td>
      </tr>
      <tr>
        <td>pi.jar</td>
	<td><a href="#">Remove</a></td>
      </tr>
    </tbody>
  </table>
</div>

${shared.footer()}
## vim:set syntax=mako:
