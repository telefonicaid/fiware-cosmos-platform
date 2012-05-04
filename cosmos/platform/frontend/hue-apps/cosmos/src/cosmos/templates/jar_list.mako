<%namespace name="shared" file="shared_components.mako" />

${shared.header("Cosmos", section="jars")}

<div class="toolbar">
    <ul class="subnav" data-filters="ArtButtonBar">
	<li>
	<a href="/cosmos/jars/upload" data-filters="ArtButton"
	   class="subnav_icon cos-upload cos-upload_jar" target="_blank"
	   data-icon-styles="{'width': 16, 'height': 16, 'top': 1}"
	   >Upload new JAR</a>
	</li>
    </ul>
</div>

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
