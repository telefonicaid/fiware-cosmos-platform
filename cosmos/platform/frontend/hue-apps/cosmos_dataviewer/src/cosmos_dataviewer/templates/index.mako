<%namespace name="shared" file="shared_components.mako" />

${shared.header("Cosmos Dataviewer")}

## use double hashes for a mako template comment

## this id in the div below ("index") is stripped by Hue.JFrame
## and passed along as the "view" argument in its onLoad event

## the class 'jframe_padded' will give the contents of your window a standard padding
<div id="index" class="view jframe_padded">
  <h2>Cosmos Dataviewer app is successfully setup!</h2>
  <p>It's now ${date}.</p>
</div>
${shared.footer()}