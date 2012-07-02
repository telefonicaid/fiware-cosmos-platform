<%namespace name="shared" file="shared_components.mako" />

${shared.header("Cosmos", section="results")}

## this id in the div below ("index") is stripped by Hue.JFrame
## and passed along as the "view" argument in its onLoad event

## the class 'jframe_padded' will give the contents of your window a standard padding
<div id="index" class="view">
  <table data-filters="HtmlTable" class="job-listing selectable sortable"
         cellpadding="0" cellspacing="0">
    <thead>
      <tr>
        <th>Name</th>
        <th>Modification time</th>
        <th>Actions</th>
      </tr>
    </thead>
    <tbody>
      % for collection in collections:
      <tr>
        <td class="collection-name">${ collection.name | h}</td>
        <td class="collection-timestamp">
        % if collection.timestamp is not None:
            ${ collection.timestamp | h }
        % else:
            Unknown
        % endif
        </td>
        <td class="collection-actions">
            % for i, link in enumerate(collection.action_links()):
            % if i > 0:
            |
            % endif
            <a href="${ link['href'] }"
               class="collection-action ${ link['class'] }"
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
## vim:set syntax=mako et:
