<%namespace name="shared" file="shared_components.mako" />

${shared.header("Cosmos", section="results")}

## the class 'jframe_padded' will give the contents of your window a standard
## padding
<div id="index" class="view jframe_padded">
    <h2>${ error_title }</h2>

    <p>${ error_details }</p>

    <p>If the error persists, please contact the service administrators.</p>
</div>

${shared.footer()}
## vim:set syntax=mako et:
